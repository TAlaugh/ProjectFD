// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/Player/WireAction2Ability.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "CableComponent.h"
#include "CableActor.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "GAS/FDGameplayStatic.h"
#include "GAS/Player/PlayerAttributeSet.h"



UWireAction2Ability::UWireAction2Ability()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ActivationRequiredTags.AddTag(UFDGameplayStatic::GetOwnerCombatTag());
	CooldownTag = FGameplayTag::RequestGameplayTag(FName("Data.Skill.Cooldown.WireAction"));
	HangOnEventTag = FGameplayTag::RequestGameplayTag(FName("Player.Event.HangOn"));
}

bool UWireAction2Ability::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;

	// 쿨다운 태그 직접 검사 & 로그
	if (ASC)
	{
		float Remaining = 0.f, Total = 0.f;
		if (UFDGameplayStatic::TryGetCooldownTimeRemaining(ASC, CooldownTag, Remaining, Total))
		{
			UE_LOG(LogTemp, Warning, TEXT("쿨타임 중: %.2f / %.2f"), Remaining, Total);
			return false;
		}
	}
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UWireAction2Ability::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = GetFDAbilitySystemComponentFromActorInfo();
	const UPlayerAttributeSet* PlayerAttrSet = ASC->GetSet<UPlayerAttributeSet>();

	// Effect Context + Spec 생성
	const FGameplayEffectContextHandle GEContextHandle = ASC->MakeEffectContext();
	const FGameplayEffectSpecHandle CooldownSpecHandle = ASC->MakeOutgoingSpec(CooldownGEClass, 1.f, GEContextHandle);

	// 쿨다운 지속시간 설정
	const float WireActionCooldown = SkillCooldown * PlayerAttrSet->GetSkillCooldown();
	CooldownSpecHandle.Data->SetDuration(WireActionCooldown, true);
	CooldownSpecHandle.Data->DynamicGrantedTags.AddTag(CooldownTag);

	if (!CommitAbility(Handle,ActorInfo,ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo,true, false);
		return;
	}

	// GE 적용
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		ASC->ApplyGameplayEffectSpecToSelf(*CooldownSpecHandle.Data.Get());
	}

	AFDPlayerCharacter* PC = Cast<AFDPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!ASC->HasMatchingGameplayTag(UFDGameplayStatic::GetOwnerCombatTag()))
	{
		PC->GetCharacterMovement()->bOrientRotationToMovement = false;
		PC->MulticastSetUseControllerRotationYaw(true);
	}

	PC->Multicast_PlaySkillSoundAtLocation(FireSound, PC->GetActorLocation());
	
	// 몽타주/이벤트 태스크 시작
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlaySkillMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, FireMontage);
		PlaySkillMontageTask->OnCompleted.AddDynamic(this, &UWireAction2Ability::LoopMontagePlay);
		PlaySkillMontageTask->OnBlendOut.AddDynamic(this, &UWireAction2Ability::LoopMontagePlay);
		PlaySkillMontageTask->OnInterrupted.AddDynamic(this, &UWireAction2Ability::K2_EndAbility);
		PlaySkillMontageTask->OnCancelled.AddDynamic(this, &UWireAction2Ability::K2_EndAbility);
		PlaySkillMontageTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitSkillEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag(FName("Event.WireAction")), nullptr, false, true);
		WaitSkillEventTask->EventReceived.AddDynamic(this, &UWireAction2Ability::LaunchWire);
		WaitSkillEventTask->ReadyForActivation();
		
		// // 입력 해제 대기 -> 취소(관성 유지)
		// UAbilityTask_WaitInputRelease* WaitRelease = UAbilityTask_WaitInputRelease::WaitInputRelease(this, true);
		// WaitRelease->OnRelease.AddDynamic(this, &UWireAction2Ability::OnInputReleased);
		// WaitRelease->ReadyForActivation();
	}
}

void UWireAction2Ability::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	AFDPlayerCharacter* PC = Cast<AFDPlayerCharacter>(GetAvatarActorFromActorInfo());
	UAbilitySystemComponent* ASC = GetFDAbilitySystemComponentFromActorInfo();

	if (!ASC->HasMatchingGameplayTag(UFDGameplayStatic::GetOwnerCombatTag()))
	{
		PC->GetCharacterMovement()->bOrientRotationToMovement = true;
		PC->MulticastSetUseControllerRotationYaw(false);
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(EndTimerHandle);
		World->GetTimerManager().ClearTimer(MoveTimerHandle);
	}
	
	EndFlying(true);
	
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UWireAction2Ability::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	AFDPlayerCharacter* PC = Cast<AFDPlayerCharacter>(GetAvatarActorFromActorInfo());
	UAbilitySystemComponent* ASC = GetFDAbilitySystemComponentFromActorInfo();

	if (!ASC->HasMatchingGameplayTag(UFDGameplayStatic::GetOwnerCombatTag()))
	{
		PC->GetCharacterMovement()->bOrientRotationToMovement = true;
		PC->MulticastSetUseControllerRotationYaw(false);
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(EndTimerHandle);
		World->GetTimerManager().ClearTimer(MoveTimerHandle);
	}
	
	// 안전 정리(이미 정리된 경우에도 중복 호출 무해)
	if (bWasCancelled)
	{
		EndFlying(true);
	}
	else
	{
		EndFlying(false);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UWireAction2Ability::LaunchWire(FGameplayEventData EventData)
{
	AFDPlayerCharacter* PC = Cast<AFDPlayerCharacter>(GetAvatarActorFromActorInfo());
	USkeletalMeshComponent* MeshComp = PC->GetMesh();
	const FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
	
	UWorld* World = PC->GetWorld();
	FCollisionQueryParams Params(SCENE_QUERY_STAT(WireActionTrace), true, PC);
	Params.bReturnPhysicalMaterial = false;

	if (World)
	{
		World->GetTimerManager().SetTimer(EndTimerHandle, this, &UWireAction2Ability::K2_EndAbility, SkillDuration, false);
	}
	
	// 조준점으로 라인 트레이스
	FVector CamLoc; FRotator CamRot;
	if (APlayerController* PCtrl = Cast<APlayerController>(PC->GetController()))
	{
		PCtrl->GetPlayerViewPoint(CamLoc, CamRot);
	}
	else
	{
		CamLoc = PC->GetActorLocation();
		CamRot = PC->GetControlRotation();
	}

	const FVector CamDir = CamRot.Vector();
	const FVector CamTraceEnd = CamLoc + CamDir * MaxWireDistance;

	FHitResult CamHit;
	World->LineTraceSingleByChannel(CamHit, CamLoc, CamTraceEnd, ECC_Visibility, Params);
	
	const FVector AimPoint = CamHit.bBlockingHit ? CamHit.ImpactPoint : CamTraceEnd;

	PC->Multicast_SpawnSkillNiagaraEffectAtLocation(WireHitEffect, AimPoint, PC->GetActorLocation());
	
	const FName AttachBone = GameplayStatics->AttachBossPart(CamHit);
	bool bAttachable = !AttachBone.IsNone();
	
	// 부착 가능 파츠 적중 여부 판정 및 타깃 갱신
	if (bAttachable && CamHit.GetActor())
	{
		bCanTriggerHangOn = true;
		HangOnTarget = CamHit.GetActor();

		// 맞은 컴포넌트가 스켈레탈 메쉬이고, 반환된 부착 뼈가 존재하면 그 뼈의 소켓 위치로 타깃 설정
		if (USkeletalMeshComponent* HitMesh = Cast<USkeletalMeshComponent>(CamHit.GetComponent()))
		{
			if (HitMesh->DoesSocketExist(AttachBone))
			{
				TargetLocation = HitMesh->GetSocketLocation(AttachBone);
				HangOnAttachBoneName = AttachBone;
			}
			else
			{
				TargetLocation = CamHit.ImpactPoint;
				HangOnAttachBoneName = NAME_None;
			}
		}
		else
		{
			TargetLocation = CamHit.ImpactPoint;
			HangOnAttachBoneName = NAME_None;
		}
	}
	else
	{
		bCanTriggerHangOn = false;
		HangOnTarget.Reset();
		TargetLocation = CamHit.bBlockingHit ? CamHit.ImpactPoint : AimPoint;
		HangOnAttachBoneName = NAME_None;
	}
	
	if (HasAuthority(&CurrentActivationInfo))
	{
		if (ShouldDebugDraw())
		{
			DrawDebugSphere(World, TargetLocation, 12.f, 12, FColor::Green, false, 3.f);
			UE_LOG(LogTemp, Log, TEXT("와이어 액션 타깃: %s, 매달리기 가능한 뼈 이름 : %s, 매달리기 여부 : %s"),
				*GetNameSafe(HangOnTarget.Get()), *HangOnAttachBoneName.ToString(), bCanTriggerHangOn ? TEXT("true") : TEXT("false"));
		}
	}
	
	// 케이블 생성: 부착 가능한 파츠면 그 컴포넌트/소켓에 바로 고정
	if (bAttachable)
	{
		if (USkeletalMeshComponent* HitMesh = Cast<USkeletalMeshComponent>(CamHit.GetComponent()))
		{
			if (HitMesh->DoesSocketExist(AttachBone))
			{
				SpawnCable(MuzzleLocation, MuzzleSocketName, TargetLocation, HitMesh, AttachBone);
			}
			else
			{
				SpawnCable(MuzzleLocation, MuzzleSocketName, TargetLocation);
			}
		}
		else
		{
			SpawnCable(MuzzleLocation, MuzzleSocketName, TargetLocation);
		}
	}
	else
	{
		SpawnCable(MuzzleLocation, MuzzleSocketName, TargetLocation);
	}
	PC->Multicast_PlaySkillSoundAtLocation(StartSound, PC->GetActorLocation());
	// 이동 시작
	BeginFlying();
	bMoving = true;
	World->GetTimerManager().SetTimer(MoveTimerHandle, this, &UWireAction2Ability::TickWireMove, TickInterval, true);
}

void UWireAction2Ability::OnInputReleased(float TimeHeld)
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UWireAction2Ability::TickWireMove()
{
	AFDCharacter* PC = Cast<AFDCharacter>(GetAvatarActorFromActorInfo());
	UCharacterMovementComponent* MovementComp = PC->GetCharacterMovement();

	// 캐릭터 전방과 타깃 방향의 수평(Yaw) 각도가 60도 이상이면 종료
	{
		FVector Forward2D = PC->GetActorForwardVector();
		Forward2D.Z = 0.f;

		FVector ToTarget2D = TargetLocation - PC->GetActorLocation();
		ToTarget2D.Z = 0.f;

		if (!Forward2D.IsNearlyZero() && !ToTarget2D.IsNearlyZero())
		{
			Forward2D.Normalize();
			ToTarget2D.Normalize();

			const float Dot = FVector::DotProduct(Forward2D, ToTarget2D);
			const float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(Dot, -1.f, 1.f)));

			if (AngleDeg >= 60.f)
			{
				EndFlying(true);
				EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
				return;
			}
		}
	}
	
	const FVector ToTarget = TargetLocation - PC->GetActorLocation() + FVector(0.f, 0.f, 300.f); // 약간 위로 보정
	const float Distance = ToTarget.Size();
	
	// 케이블을 항상 도착지점까지 팽팽하게 보이도록 길이 갱신
	if (CableActor.IsValid())
	{
		if (UCableComponent* CC = CableActor->CableComponent)
		{
			const FVector StartLoc = (PC->GetMesh() && PC->GetMesh()->DoesSocketExist(MuzzleSocketName))
				? PC->GetMesh()->GetSocketLocation(MuzzleSocketName)
				: PC->GetActorLocation();
			CC->CableLength = FVector::Distance(StartLoc, TargetLocation);
		}
	}
	
	// 도달 판정
	if (Distance <= AcceptRadius + 200.f) // 약간 여유있게
	{
		// 적중 대상이 부착 가능한 파츠였으면 HangOn 이벤트 전송
		if (bCanTriggerHangOn)
		{
			FGameplayEventData EventData;
			EventData.EventTag = HangOnEventTag;
			EventData.Instigator = PC;
			EventData.Target = HangOnTarget.Get();

			FHitResult HR;
			HR.BoneName = HangOnAttachBoneName;

			AActor* TargetActor = HangOnTarget.Get();
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

			if (!TargetASC->HasMatchingGameplayTag(UFDGameplayStatic::PlayerAttachParts()))
			{
				EventData.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromHitResult(HR);
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PC, HangOnEventTag, EventData);
			}
			UE_LOG(LogTemp, Warning, TEXT("%s, %s"), *HR.BoneName.ToString(), *EventData.Target->GetName());
		}

		EndFlying(false);
		K2_EndAbility();
		return;
	}

	const FVector Dir = ToTarget.GetSafeNormal();

	// 거리 기반 가속 스케일 (가까워질수록 감소)
	float AccelScale = 1.f;
	if (Distance <= DecelStartDistance)
	{
		const float Ratio = FMath::Clamp(Distance / FMath::Max(DecelStartDistance, KINDA_SMALL_NUMBER), 0.f, 1.f);
		AccelScale = FMath::Clamp(Ratio, MinAccelScale, 1.f);
	}

	// 가속 적용
	FVector NewVelocity = MovementComp->Velocity + (Dir * (InitialAcceleration * AccelScale) * TickInterval);

	// 최고 속도 제한
	const float Speed = NewVelocity.Size();
	if (Speed > MaxSpeed)
	{
		NewVelocity = NewVelocity.GetSafeNormal() * MaxSpeed;
	}

	// 비행 모드 유지
	if (MovementComp->MovementMode != MOVE_Flying)
	{
		MovementComp->SetMovementMode(MOVE_Flying);
	}
	
	MovementComp->Velocity = NewVelocity;
}

void UWireAction2Ability::SpawnCable(const FVector& StartWorldLoc, const FName& StartSocketName,
	const FVector& EndWorldLoc, USceneComponent* OptionalEndComp, FName OptionalEndSocket)
{
	DestroyCable();

	ACharacter* Character = Cast<ACharacter>(CurrentActorInfo ? CurrentActorInfo->AvatarActor.Get() : nullptr);
	UWorld* World = GetWorld();
	
	// 케이블 액터 생성(시작점은 캐릭터 메시에 소켓 부착)
	ACableActor* Cable = World->SpawnActor<ACableActor>(CableActorClass ? *CableActorClass : ACableActor::StaticClass(), StartWorldLoc, FRotator::ZeroRotator);
	Cable->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, StartSocketName);
	
	// GetCableComponent() 대신 공개 멤버 사용
	if (UCableComponent* CC = Cable->CableComponent)
	{
		// 시작점 부착
		CC->bAttachStart = true;
		
		// 와이어느낌(팽팽하고 곧게)
		CC->CableGravityScale = 0.f;
		CC->bEnableStiffness = true;
		CC->bUseSubstepping = true;
		CC->SubstepTime = 1.f / 120.f;
		CC->SolverIterations = 16;
		CC->bEnableCollision = false;
		CC->bSkipCableUpdateWhenNotVisible = false;

		// 가시성/두께/세그먼트
		CC->SetVisibility(true, true);
		CC->CableWidth = 4.0f;     // 보기 좋은 두께로 조정
		CC->NumSegments = 1;       // 낮을수록 직선에 가까움

		// 엔드 오프셋 초기화(밀림 방지)
		CC->EndLocation = FVector::ZeroVector;
		
		// 렌더 프록시 재생성(필수)
		CC->MarkRenderStateDirty();
	}

	// OptionalEndComp이 있으면 해당 컴포넌트/소켓에 끝을 고정(앵커 생성 생략)
	if (OptionalEndComp)
	{
		if (UCableComponent* CC = Cable->CableComponent)
		{
			CC->bAttachEnd = true;
			CC->SetAttachEndToComponent(OptionalEndComp, OptionalEndSocket);
			CC->EndLocation = FVector::ZeroVector;

			// 초기 길이는 실제 월드 거리로 설정
			FVector EndLocForLength = EndWorldLoc;
			if (USkeletalMeshComponent* Skel = Cast<USkeletalMeshComponent>(OptionalEndComp))
			{
				if (!OptionalEndSocket.IsNone() && Skel->DoesSocketExist(OptionalEndSocket))
				{
					EndLocForLength = Skel->GetSocketLocation(OptionalEndSocket);
				}
				else
				{
					EndLocForLength = OptionalEndComp->GetComponentLocation();
				}
			}
			else
			{
				EndLocForLength = OptionalEndComp->GetComponentLocation();
			}

			CC->CableLength = FVector::Distance(StartWorldLoc, EndLocForLength);
			CC->MarkRenderStateDirty();
		}

		CableEndAnchor = nullptr;
	}
	else
	{
		// 기존: 끝점용 임시 앵커 액터 생성
		AActor* EndAnchor = World->SpawnActor<AActor>(AActor::StaticClass(), EndWorldLoc, FRotator::ZeroRotator);
		if (EndAnchor)
		{
			USceneComponent* AnchorRoot = EndAnchor->GetRootComponent();
			if (!AnchorRoot)
			{
				AnchorRoot = NewObject<USceneComponent>(EndAnchor, TEXT("WireEndRoot"));
				AnchorRoot->RegisterComponent();
				EndAnchor->SetRootComponent(AnchorRoot);
			}
			AnchorRoot->SetWorldLocation(EndWorldLoc);
			AnchorRoot->SetWorldRotation(FRotator::ZeroRotator);
			AnchorRoot->SetWorldScale3D(FVector::OneVector);

			if (UCableComponent* CC = Cable->CableComponent)
			{
				CC->bAttachEnd = true;
				CC->SetAttachEndToComponent(AnchorRoot, NAME_None);
				CC->EndLocation = FVector::ZeroVector;
				CC->CableLength = FVector::Distance(StartWorldLoc, EndWorldLoc);
				CC->MarkRenderStateDirty();
			}
		}

		CableEndAnchor = EndAnchor;
	}

	CableActor = Cable;
}

// DestroyCable: 부착 해제 후 렌더 상태 더티 마킹 -> 파괴
void UWireAction2Ability::DestroyCable()
{
	if (CableActor.IsValid())
	{
		if (UCableComponent* CC = CableActor->CableComponent)
		{
			CC->bAttachStart = false;
			CC->bAttachEnd = false;
			CC->SetVisibility(false, true);
			CC->MarkRenderStateDirty();
		}
		CableActor->Destroy();
	}

	if (CableEndAnchor.IsValid())
	{
		CableEndAnchor->Destroy();
	}

	CableActor.Reset();
	CableEndAnchor.Reset();
}

void UWireAction2Ability::BeginFlying()
{
	ACharacter* Character = Cast<ACharacter>(CurrentActorInfo ? CurrentActorInfo->AvatarActor.Get() : nullptr);
	if (!Character) return;

	if (UCharacterMovementComponent* Move = Character->GetCharacterMovement())
	{
		// 중력 영향 배제하고 비행
		Move->SetMovementMode(MOVE_Flying);
	}
}

void UWireAction2Ability::EndFlying(bool bKeepMomentum)
{
	// 틱 정지 및 케이블 제거
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(MoveTimerHandle);
	}
	bMoving = false;
	DestroyCable();

	ACharacter* Character = Cast<ACharacter>(CurrentActorInfo ? CurrentActorInfo->AvatarActor.Get() : nullptr);
	if (!Character) return;

	if (UCharacterMovementComponent* Move = Character->GetCharacterMovement())
	{
		// 관성 유지: 낙하로 전환해 현재 속도로 계속 이동
		Move->SetMovementMode(MOVE_Falling);
		
		const float Speed = Move->Velocity.Size();
		if (Speed > MaxKeepSpeed)
		{
			Move->Velocity = Move->Velocity.GetSafeNormal() * MaxKeepSpeed;
		}

	}
}

void UWireAction2Ability::LoopMontagePlay()
{
	UAbilityTask_PlayMontageAndWait* PlaySkillMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, LoopMontage);
	PlaySkillMontageTask->OnCompleted.AddDynamic(this, &UWireAction2Ability::K2_EndAbility);
	PlaySkillMontageTask->OnBlendOut.AddDynamic(this, &UWireAction2Ability::K2_EndAbility);
	PlaySkillMontageTask->OnInterrupted.AddDynamic(this, &UWireAction2Ability::K2_EndAbility);
	PlaySkillMontageTask->OnCancelled.AddDynamic(this, &UWireAction2Ability::K2_EndAbility);
	PlaySkillMontageTask->ReadyForActivation();
}
