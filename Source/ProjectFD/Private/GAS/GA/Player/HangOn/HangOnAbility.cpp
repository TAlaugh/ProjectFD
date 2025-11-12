// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/Player/HangOn/HangOnAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Equipment/EquipmentActor.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GAS/FDGameplayStatic.h"
#include "GAS/GA/Player/WireActionAbility.h"


UHangOnAbility::UHangOnAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
	HangOnAttackAbilityTag = FGameplayTag::RequestGameplayTag(FName("Ability.HangOn.Attack"));
	HangOnGrabAbilityTag = FGameplayTag::RequestGameplayTag(FName("Ability.HangOn.Grab"));
	HangOnGrabEventTag = FGameplayTag::RequestGameplayTag(FName("Event.HangOn.Grab"));
}

void UHangOnAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = GetFDAbilitySystemComponentFromActorInfo();
	PC = Cast<AFDPlayerCharacter>(GetAvatarActorFromActorInfo());
	
	// Effect Context + Spec 생성
	const FGameplayEffectContextHandle GEContextHandle = ASC->MakeEffectContext();
	const FGameplayEffectSpecHandle DurationSpecHandle = ASC->MakeOutgoingSpec(DurationGEClass, 1.f, GEContextHandle);

	// 초기 지속시간 설정
	DurationSpecHandle.Data->SetDuration(HangOnDuration, true);
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// GE 적용 시 핸들 보관(실시간 속도 제어를 위해)
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		DurationEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*DurationSpecHandle.Data.Get());
	}

	if (ASC)
	{
		// 태그 변화 이벤트 등록
		auto& HangOnGrabEvent =
		ASC->RegisterGameplayTagEvent(HangOnGrabEventTag, EGameplayTagEventType::NewOrRemoved);
		HangOnGrabTagChangedHandle =
			HangOnGrabEvent.AddUObject(this, &UHangOnAbility::OnHangOnGrabTagChanged);
		
		auto& BodySwingEvent =
			ASC->RegisterGameplayTagEvent(UFDGameplayStatic::BodySwing(), EGameplayTagEventType::NewOrRemoved);
		HangOnBodySwingTagChangedHandle =
			BodySwingEvent.AddUObject(this, &UHangOnAbility::OnHangOnBodySwingTagChanged);
	}
	
	// WireActionAbility에서 보낸 값 꺼내기 TargetActor / BoneName 파싱
	const AActor* TargetActor = TriggerEventData ? TriggerEventData->Target.Get() : nullptr;
	
	if (TriggerEventData && TriggerEventData->TargetData.Num() > 0)
	{
		const FGameplayAbilityTargetData* TD = TriggerEventData->TargetData.Get(0);
		if (TD)
		{
			if (const FHitResult* HR = TD->GetHitResult())
			{
				HangOnActorBoneName = HR->BoneName;
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("HangOn 이벤트 수신: Target=%s, Bone=%s"),
		*GetNameSafe(TargetActor), *HangOnActorBoneName.ToString());

	
	// 예: 부착 지점이 유효하면 그 위치를 사용한 처리
	if (TargetActor)
	{
		if (USkeletalMeshComponent* Skel = TargetActor->FindComponentByClass<USkeletalMeshComponent>())
		{
			if (!HangOnActorBoneName.IsNone() && Skel->DoesSocketExist(HangOnActorBoneName))
			{
				// 대상 컴포넌트를 캐싱해 두고 실제 부착 진행
				HangOnTargetSkel = Skel;
				
				if (PC)
				{
					PC->Server_AttachToBossSocket(const_cast<AActor*>(TargetActor), HangOnActorBoneName);
				}
				// 서버/권한 측에서 타겟에 태그 GE 적용
				if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
				{
					ApplyAttachTagToTarget(const_cast<AActor*>(TargetActor));
				}
			}
		}
	}
	
	///
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayHangOnStartMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, HangOnStartMontage);
		PlayHangOnStartMontageTask->OnCompleted.AddDynamic(this, &UHangOnAbility::LoopHangOnMontagePlay);
		PlayHangOnStartMontageTask->OnBlendOut.AddDynamic(this, &UHangOnAbility::LoopHangOnMontagePlay);
		PlayHangOnStartMontageTask->ReadyForActivation();
	}
	
	AimStartArmLength = PC->CameraBoom->TargetArmLength;

	PC->CameraBoom->TargetArmLength = AimStartArmLength + 200.f;

	// 공격 어빌리티 종료 감지 등록
	if (ASC)
	{
		OnAbilityEndedHandle = ASC->OnAbilityEnded.AddUObject(this, &UHangOnAbility::OnASCAbilityEnded);
	}
	
	// 지속 시간 후 어빌리티 종료
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			DurationTimerHandle,
			this,
			&UHangOnAbility::K2_EndAbility,
			HangOnDuration,
			false);
		//-------------------------------------------
		StartCooldownLogTicker();
		//-------------------------------------------
	}
	
}

void UHangOnAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	RemoveAttachTagFromTarget();
	PC->CameraBoom->TargetArmLength = AimStartArmLength;
	// 델리게이트 언바인딩
	if (UAbilitySystemComponent* ASC = GetFDAbilitySystemComponentFromActorInfo())
	{
		if (OnAbilityEndedHandle.IsValid())
		{
			ASC->OnAbilityEnded.Remove(OnAbilityEndedHandle);
			OnAbilityEndedHandle.Reset();
		}
	}

	if (UAbilitySystemComponent* ASC = GetFDAbilitySystemComponentFromActorInfo())
	{
		// Grab 이벤트 태그 변화 언바인딩
		ASC->RegisterGameplayTagEvent(HangOnGrabEventTag, EGameplayTagEventType::NewOrRemoved)
			.Remove(HangOnGrabTagChangedHandle);
		HangOnGrabTagChangedHandle.Reset();
		ASC->RegisterGameplayTagEvent(UFDGameplayStatic::BodySwing(), EGameplayTagEventType::NewOrRemoved)
			.Remove(HangOnBodySwingTagChangedHandle);
		HangOnBodySwingTagChangedHandle.Reset();
	}
	//-------------------------------------------
	// 로그 틱 정리
	StopCooldownLogTicker();
	//-------------------------------------------
	// 부착 해제 및 이동 복구
	DetachFromHangOnBone();
	
	// 종료 몽타주 재생
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (HangOnEndMontage)
		{
			UAbilityTask_PlayMontageAndWait* PlayHangOnEndMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, HangOnEndMontage);
			PlayHangOnEndMontageTask->ReadyForActivation();
		}
	}
	
	// 종료 타이머 정리
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DurationTimerHandle);
	}
	// GE 핸들 리셋
	DurationEffectHandle.Invalidate();
	HangOnTimeScale = 1.f;
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


void UHangOnAbility::LoopHangOnMontagePlay()
{
	UAbilityTask_PlayMontageAndWait* PlayHangOnLoopMontageTask;

	
	PlayHangOnLoopMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, HangOnLoopMontage);
//	PlayHangOnLoopMontageTask->OnInterrupted.AddDynamic(this, &UHangOnAbility::K2_EndAbility);
	PlayHangOnLoopMontageTask->OnCancelled.AddDynamic(this, &UHangOnAbility::K2_EndAbility);
	PlayHangOnLoopMontageTask->ReadyForActivation();
}

void UHangOnAbility::AttachToHangOnBone()
{

	// // 소켓에 스냅 부착(스케일은 유지)
	// USceneComponent* Capsule = PC->GetCapsuleComponent();
	// if (Capsule)
	// {
	// 	PC->bUseControllerRotationYaw = false;
	// //	PC->GetCapsuleComponent()->SetSimulatePhysics(false);
	// 	PC->GetCharacterMovement()->SetMovementMode(MOVE_None);
	// 	PC->GetCharacterMovement()->GravityScale = 0.f;
	// 	Capsule->AttachToComponent(HangOnTargetSkel.Get(),
	// 		FAttachmentTransformRules::SnapToTargetIncludingScale,
	// 		HangOnActorBoneName);
	// }
	if (!PC)
	{
		PC = Cast<AFDPlayerCharacter>(GetAvatarActorFromActorInfo());
	}
	AActor* BossActor = HangOnTargetSkel.IsValid() ? HangOnTargetSkel->GetOwner() : nullptr;
	if (PC && BossActor && !HangOnActorBoneName.IsNone())
	{
		PC->Server_AttachToBossSocket(BossActor, HangOnActorBoneName);
	}
}


void UHangOnAbility::DetachFromHangOnBone()
{
	// // 복구
	// UCapsuleComponent* Capsule = PC->GetCapsuleComponent();
	// if (Capsule)
	// {
	// 	PC->bUseControllerRotationYaw = true;
	// //	PC->GetCapsuleComponent()->SetSimulatePhysics(true);
	// 	PC->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	// 	PC->GetCharacterMovement()->GravityScale = 1.f;
	// 	Capsule->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	// 	Capsule->SetRelativeRotation(FRotator::ZeroRotator);
	// }
	if (!PC)
	{
		PC = Cast<AFDPlayerCharacter>(GetAvatarActorFromActorInfo());
	}
	if (PC)
	{
		PC->Server_DetachFromBossSocket();
	}
	RemoveAttachTagFromTarget();
	
	/////////////////////////////////////////////////////////
	PC->LaunchCharacter(-PC->GetActorForwardVector() * 400.f + FVector(0.f, 0.f, 200.f), true, true);

	HangOnTargetSkel = nullptr;
}

void UHangOnAbility::OnASCAbilityEnded(const FAbilityEndedData& Data)
{
	// 자기 자신 종료나 잘못된 포인터 방지
	if (!Data.AbilityThatEnded || Data.AbilityThatEnded == this)
	{
		return;
	}

	// 어빌리티 종료만 처리
	if (Data.AbilityThatEnded->AbilityTags.HasTagExact(HangOnAttackAbilityTag) || 
		Data.AbilityThatEnded->AbilityTags.HasTagExact(HangOnGrabAbilityTag))
	{
		// 아직 HangOn 어빌리티가 활성이라면 다음 틱에 루프 몽타주 복구
		if (IsActive())
		{
			if (UWorld* World = GetWorld())
			{
				World->GetTimerManager().SetTimerForNextTick(
					FTimerDelegate::CreateUObject(this, &UHangOnAbility::LoopHangOnMontagePlay));
			}
		}
	}
}

// 실시간 쿨다운(지속시간) 속도를 변경한다.
void UHangOnAbility::SetHangOnCooldownSpeed(float NewSpeed)
{
	// 0에 수렴하면 타이머 동작이 불가능하므로 하한 보정
	const float Clamped = FMath::Max(0.01f, NewSpeed);

	if (FMath::IsNearlyEqual(Clamped, HangOnTimeScale))
	{
		return;
	}

	UAbilitySystemComponent* ASC = GetFDAbilitySystemComponentFromActorInfo();
	
	// 현재 타이머 남은 시간 기준으로 새 남은 시간 계산
	if (UWorld* World = GetWorld())
	{
		float Remaining = World->GetTimerManager().GetTimerRemaining(DurationTimerHandle);
		if (Remaining > 0.f)
		{
			const float Ratio = HangOnTimeScale / Clamped; // 기존 대비 배율
			const float NewRemaining = Remaining * Ratio;

			// 1) 종료 타이머 재설정
			RescheduleEndTimer(NewRemaining);

			// 2) GE 재적용으로 남은 시간 반영 (UE 5.4에는 SetActiveGameplayEffectDuration 미제공)
			if (DurationEffectHandle.IsValid() &&
				DurationGEClass &&
				HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
			{
				// 기존 GE 제거
				ASC->RemoveActiveGameplayEffect(DurationEffectHandle);

				// 남은 시간을 새 지속시간으로 설정해 재적용
				const FGameplayEffectContextHandle Ctx = ASC->MakeEffectContext();
				FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(DurationGEClass, 1.f, Ctx);
				if (Spec.IsValid())
				{
					Spec.Data->SetDuration(NewRemaining, true);
					DurationEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
				}
			}

			HangOnTimeScale = Clamped;
		}
	}
}

void UHangOnAbility::RescheduleEndTimer(float NewRemainingSeconds)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DurationTimerHandle);
		World->GetTimerManager().SetTimer(
			DurationTimerHandle,
			this,
			&UHangOnAbility::K2_EndAbility,
			FMath::Max(0.001f, NewRemainingSeconds),
			false);
	}
}

void UHangOnAbility::OnHangOnGrabTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	OnHangOnCooldownFromTags();
}

void UHangOnAbility::OnHangOnBodySwingTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	OnHangOnCooldownFromTags();
}

void UHangOnAbility::OnHangOnCooldownFromTags()
{
	UAbilitySystemComponent* ASC = GetFDAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	const bool bGrab = ASC->HasMatchingGameplayTag(HangOnGrabEventTag);
	const bool bSwing = ASC->HasMatchingGameplayTag(UFDGameplayStatic::BodySwing());

	float NewSpeed = 1.0f;
	if (bGrab && bSwing)
	{
		NewSpeed = 1.0f;
	}
	else if (bGrab)
	{
		NewSpeed = 0.5f;
	}
	else if (bSwing)
	{
		NewSpeed = 2.0f;
	}
	SetHangOnCooldownSpeed(NewSpeed);
}

void UHangOnAbility::ApplyAttachTagToTarget(AActor* TargetActor)
{
	if (!AttachPlayerPartsTagGEClass || !TargetActor)
	{
		return;
	}

	UAbilitySystemComponent* SourceASC = GetFDAbilitySystemComponentFromActorInfo();
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!SourceASC || !TargetASC)
	{
		return;
	}

	// 서버에서만 적용 권장
	if (!HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
	{
		return;
	}

	const FGameplayEffectContextHandle Ctx = SourceASC->MakeEffectContext();
	FGameplayEffectSpecHandle Spec = SourceASC->MakeOutgoingSpec(AttachPlayerPartsTagGEClass, 1.f, Ctx);
	if (Spec.IsValid())
	{
		TargetAttachTagGEHandle = SourceASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
		TargetASCWeak = TargetASC;
	}
}

void UHangOnAbility::RemoveAttachTagFromTarget()
{
	if (TargetAttachTagGEHandle.IsValid())
	{
		if (UAbilitySystemComponent* TargetASC = TargetASCWeak.Get())
		{
			TargetASC->RemoveActiveGameplayEffect(TargetAttachTagGEHandle);
		}
		TargetAttachTagGEHandle.Invalidate();
	}
	TargetASCWeak.Reset();
}

void UHangOnAbility::StartCooldownLogTicker()
{
	// 서버/권한에서만 로그 타이머 시작
	if (!CurrentActorInfo || !CurrentActorInfo->IsNetAuthority())
	{
		return;
	}
	
	if (UWorld* World = GetWorld())
	{
		FTimerManager& TM = World->GetTimerManager();

		// 중복 시작 방지
		if (bCooldownLogTickerActive || TM.IsTimerActive(CooldownLogTimerHandle))
		{
			return;
		}

		bCooldownLogTickerActive = true;

		TM.SetTimer(
		 CooldownLogTimerHandle,
		 this,
		 &UHangOnAbility::LogCooldownTick,
		 FMath::Max(0.01f, CooldownLogInterval),
		 true);

		// 즉시 한 번 출력
		LogCooldownTick();
	}
}

void UHangOnAbility::StopCooldownLogTicker()
{
	if (UWorld* World = GetWorld())
	{
		FTimerManager& TM = World->GetTimerManager();
		if (TM.IsTimerActive(CooldownLogTimerHandle))
		{
			TM.ClearTimer(CooldownLogTimerHandle);
		}
	}
	bCooldownLogTickerActive = false;
}

void UHangOnAbility::LogCooldownTick()
{
	// 서버에서만 로그 출력
	if (!CurrentActorInfo || !CurrentActorInfo->IsNetAuthority())
	{
		return;
	}
	
	// 타겟이 파괴 태그를 가지고 있으면 즉시 종료
	UAbilitySystemComponent* TargetASC = TargetASCWeak.IsValid() ? TargetASCWeak.Get() : nullptr;
	if (!TargetASC)
	{
		if (USkeletalMeshComponent* Skel = HangOnTargetSkel.Get())
		{
			if (AActor* TargetActor = Skel->GetOwner())
			{
				TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
			}
		}
	}
	static const FGameplayTag DestroyPartTag = FGameplayTag::RequestGameplayTag(FName("Enemy.Boss.Destroy.AM"));
	if (TargetASC && TargetASC->HasMatchingGameplayTag(DestroyPartTag))
	{
		K2_EndAbility();
		return;
	}
	/////////////////////////////////////////////////
	// // 부착 대상 컴포넌트가 파괴되었으면 즉시 종료
	// if (!HangOnTargetSkel.IsValid())
	// {
	// 	K2_EndAbility();
	// 	return;
	// }
	
	if (UWorld* World = GetWorld())
	{
		const float Remaining = World->GetTimerManager().GetTimerRemaining(DurationTimerHandle);
		if (Remaining <= 0.f)
		{
			// 만료 시 자동 정지
			StopCooldownLogTicker();
			return;
		}
		
		UE_LOG(LogTemp, Log, TEXT("[HangOn] Cooldown remaining: %.2fs (x%.2f)"),
			   Remaining, HangOnTimeScale);
	}
}