// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/Player/BunnySkillFourAbility.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "Equipment/EquipmentActor.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "GAS/FDGameplayStatic.h"
#include "GAS/Player/PlayerAttributeSet.h"
#include "Kismet/GameplayStatics.h"

UBunnySkillFourAbility::UBunnySkillFourAbility()
{
	ActivationRequiredTags.AddTag(UFDGameplayStatic::GetOwnerCombatTag());
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	CooldownTag = FGameplayTag::RequestGameplayTag(TEXT("Data.Skill.Cooldown.4"));
}

bool UBunnySkillFourAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
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

void UBunnySkillFourAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = GetFDAbilitySystemComponentFromActorInfo();
	const UPlayerAttributeSet* PlayerAttrSet = ASC->GetSet<UPlayerAttributeSet>();
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	DamageMultiplier = 1.f;
	StackAccumulated = 0.f;
	
	// 비용 계산 & 자원 체크
	const float BunnySkillFourCost = CostAmount * PlayerAttrSet->GetSkillCost();
	if (PlayerAttrSet->GetSpecialResource() < BunnySkillFourCost )
	{
		UE_LOG(LogTemp, Warning, TEXT("전기력이 부족합니다."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	
	// Effect Context + Spec 생성
	const FGameplayEffectContextHandle GEContextHandle = ASC->MakeEffectContext();
	const FGameplayEffectSpecHandle CostSpecHandle = ASC->MakeOutgoingSpec(CostGEClass, 1.f, GEContextHandle);
	const FGameplayEffectSpecHandle CooldownSpecHandle = ASC->MakeOutgoingSpec(CooldownGEClass, 1.f, GEContextHandle);

	// 쿨다운 지속시간 설정
	const float BunnySkillFourCooldown = SkillCooldown * PlayerAttrSet->GetSkillCooldown();
	CooldownSpecHandle.Data->SetDuration(BunnySkillFourCooldown, true);
	CooldownSpecHandle.Data->DynamicGrantedTags.AddTag(CooldownTag);
	
	
	// SetByCaller를 이용해 CostAmount 설정
	const FGameplayTag CostTag = FGameplayTag::RequestGameplayTag(TEXT("Data.Skill.Cost"));
	CostSpecHandle.Data->SetSetByCallerMagnitude(CostTag, -BunnySkillFourCost);
	
	
	if (!CommitAbility(Handle,ActorInfo,ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo,true, false);
		return;
	}

	// GE 적용
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		ASC->ApplyGameplayEffectSpecToSelf(*CostSpecHandle.Data.Get());
		ASC->ApplyGameplayEffectSpecToSelf(*CooldownSpecHandle.Data.Get());
	}

	AFDPlayerCharacter* PC = Cast<AFDPlayerCharacter>(GetAvatarActorFromActorInfo());
	
	// 0.1초 뒤에 쿨다운 알림 실행
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(CooldownTimerHandle, [this]()
			{
				AFDPlayerCharacter* PC = Cast<AFDPlayerCharacter>(GetAvatarActorFromActorInfo());
				if (HasAuthority(&CurrentActivationInfo))
				{
					PC->NotifySkillCoolDown(CooldownTag);
				}
			},
			0.1f,
			false);
	}
	
	PC->GetCharacterMovement()->MaxWalkSpeed = 200.0f;

	if (PC->GetWeaponActor())
	{
		PC->GetWeaponActor()->SetWeaponVisibility(false);
	}

	if (!ASC->HasMatchingGameplayTag(UFDGameplayStatic::GetOwnerCombatTag()))
	{
		PC->GetCharacterMovement()->bOrientRotationToMovement = false;
		PC->MulticastSetUseControllerRotationYaw(true);
	}

	PC->Multicast_PlaySkillSoundAtLocation(SkillFourSoundStart, PC->GetActorLocation());
	
	// 몽타주/이벤트 태스크 시작
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlaySkillMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, SkillMontage);
		PlaySkillMontageTask->OnBlendOut.AddDynamic(this, &UBunnySkillFourAbility::K2_EndAbility);
		PlaySkillMontageTask->OnCompleted.AddDynamic(this, &UBunnySkillFourAbility::K2_EndAbility);
		PlaySkillMontageTask->OnInterrupted.AddDynamic(this, &UBunnySkillFourAbility::K2_EndAbility);
		PlaySkillMontageTask->OnCancelled.AddDynamic(this, &UBunnySkillFourAbility::K2_EndAbility);
		PlaySkillMontageTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitSkillEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag(FName("Event.BunnySkill.Four")), nullptr, false, true);
		WaitSkillEventTask->EventReceived.AddDynamic(this, &UBunnySkillFourAbility::StartTick);
		WaitSkillEventTask->ReadyForActivation();
	}
}

void UBunnySkillFourAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	AFDPlayerCharacter* PC = Cast<AFDPlayerCharacter>(GetAvatarActorFromActorInfo());
	UAbilitySystemComponent* ASC = GetFDAbilitySystemComponentFromActorInfo();
	const FGameplayTag SpeedTag = FGameplayTag::RequestGameplayTag(TEXT("Ability.BunnySkill.Two"));
	const bool bHasSpeedTag = ASC->HasMatchingGameplayTag(SpeedTag);
	PC->GetCharacterMovement()->MaxWalkSpeed = bHasSpeedTag ? 1200.0f : 800.0f;
	if (PC->GetWeaponActor())
	{
		PC->GetWeaponActor()->SetWeaponVisibility(true);
	}

	if (!ASC->HasMatchingGameplayTag(UFDGameplayStatic::GetOwnerCombatTag()))
	{
		PC->GetCharacterMovement()->bOrientRotationToMovement = true;
		PC->MulticastSetUseControllerRotationYaw(false);
	}
	PC->Multicast_PlaySkillSoundAtLocation(SkillFourSoundEnd, PC->GetActorLocation());
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBunnySkillFourAbility::InputPressed(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	// 재눌러 취소
	if (IsActive())
	{
		K2_EndAbility();
	}
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
}

void UBunnySkillFourAbility::OnTick()
{
	// 끝난 뒤에는 더 이상 루프하지 않음
	if (!IsActive())
	{
		return;
	}

	AFDPlayerCharacter* PC = Cast<AFDPlayerCharacter>(GetAvatarActorFromActorInfo());
	PC->GetCharacterMovement()->MaxWalkSpeed = 200.0f;
	PC->Multicast_PlaySkillSoundAtLocation(SkillFourSoundLoop, PC->GetActorLocation());
	// 유지 시간 누적 및 배율 상승
	StackAccumulated += TickInterval;
	if (StackAccumulated >= StackInterval)
	{
		const int32 Steps = static_cast<int32>(StackAccumulated / StackInterval);
		DamageMultiplier += 0.2f * static_cast<float>(Steps);
		StackAccumulated -= StackInterval * static_cast<float>(Steps);
	}

	// 비용 지불 및 피해 적용, 실패 시 종료
	if (!PayCostAndDealDamage())
	{
		K2_EndAbility();
		return;
	}

	// 다음 틱 예약
	StartTick(FGameplayEventData());
}

void UBunnySkillFourAbility::StartTick(FGameplayEventData EventData)
{
	// TickInterval 후 OnTick 호출, 내부에서 재스케줄
	UAbilityTask_WaitDelay* DelayTask = UAbilityTask_WaitDelay::WaitDelay(this, TickInterval);
	if (DelayTask)
	{
		DelayTask->OnFinish.AddDynamic(this, &UBunnySkillFourAbility::OnTick);
		DelayTask->ReadyForActivation();
	}
}

bool UBunnySkillFourAbility::DoLineTrace(const FGameplayAbilityActorInfo* ActorInfo, FHitResult& OutHit,
                                         FVector& OutStart, FVector& OutEnd) const
{
	OutHit = FHitResult{};
	OutStart = FVector::ZeroVector;
	OutEnd = FVector::ZeroVector;

	if (!ActorInfo) return false;

	const ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!Character) return false;

	UWorld* World = ActorInfo->AvatarActor.IsValid() ? ActorInfo->AvatarActor->GetWorld() : nullptr;
	if (!World) return false;
	

	auto TryGetWorldLocation = [](const USkeletalMeshComponent* Skel, const FName& Name, FVector& OutLoc) -> bool
	{
		if (!Skel) return false;

		const bool bHasSocket = Skel->DoesSocketExist(Name);
		const int32 BoneIdx = Skel->GetBoneIndex(Name);

		if (bHasSocket)
		{
			OutLoc = Skel->GetSocketLocation(Name);
			return true;
		}

		if (BoneIdx != INDEX_NONE)
		{
			FTransform BoneTM = Skel->GetBoneTransform(BoneIdx) * Skel->GetComponentToWorld();
			OutLoc = BoneTM.GetLocation();
			return true;
		}
		return false;
	};

	// 시작점: 무기 메시의 총구 소켓/본 -> 캐릭터 메시의 소켓/본 -> 캐릭터 위치
	FVector Start = Character->GetActorLocation();

	if (!TryGetWorldLocation(Character->GetMesh(), MuzzleSocketName, Start))
	{
		Start = Character->GetActorLocation();
	}

    // 방향: 컨트롤러 뷰 회전 사용
    FVector CamLoc;
    FRotator CamRot = Character->GetActorRotation();
    if (AController* PC = Character->GetController())
    {
        PC->GetPlayerViewPoint(CamLoc, CamRot);
    }
    CamLoc = Start; // 시작점은 총구 기준

    const FVector Dir = CamRot.Vector();
    const FVector End = Start + Dir * BunnySkillFourTraceDistance;

    FCollisionQueryParams Params(SCENE_QUERY_STAT(BunnySkillFourTrace), false);
    Params.AddIgnoredActor(Character);
    Params.bReturnPhysicalMaterial = true;

    const bool bHit = World->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, Params);

    // 디버그
    if (bDrawTraceDebug)
    {
        const FVector LineEnd = bHit ? OutHit.ImpactPoint : End;
        const FColor LineColor = bHit ? TraceColorHit : TraceColorMiss;
        DrawDebugLine(World, Start, LineEnd, LineColor, false, TraceDebugTime, 0, TraceThickness);

        if (bHit)
        {
            DrawDebugPoint(World, OutHit.ImpactPoint, ImpactPointSize, TraceColorHit, false, TraceDebugTime);
            DrawDebugDirectionalArrow(World, OutHit.ImpactPoint, OutHit.ImpactPoint + OutHit.ImpactNormal * 30.f, 10.f, TraceColorHit, false, TraceDebugTime, 0, 1.0f);
        }
    }
	
	if (SkillFourNiagaraSystem)
	{
		AFDPlayerCharacter* PC = Cast<AFDPlayerCharacter>(GetAvatarActorFromActorInfo());
		PC->Multicast_SpawnSkillNiagaraEffectAtLocation(SkillFourNiagaraSystem, Start, End);
	}
	
    OutStart = Start;
    OutEnd = End;
    return bHit;
}
bool UBunnySkillFourAbility::PayCostAndDealDamage()
{
	UAbilitySystemComponent* ASC = GetFDAbilitySystemComponentFromActorInfo();
	if (!ASC) return false;

	const UPlayerAttributeSet* PlayerAttrSet = ASC->GetSet<UPlayerAttributeSet>();
	if (!PlayerAttrSet) return false;

	if (PlayerAttrSet->GetSpecialResource() < EnergyCostPerSecond)
	{
		UE_LOG(LogTemp, Warning, TEXT("전기력이 부족하여 스킬이 종료됩니다."));
		return false;
	}

	// 틱당 비용 지불
	if (CostGEClass && HasAuthority(&CurrentActivationInfo))
	{
		const FGameplayEffectContextHandle GEContextHandle = ASC->MakeEffectContext();
		FGameplayEffectSpecHandle CostSpecHandle = ASC->MakeOutgoingSpec(CostGEClass, 1.f, GEContextHandle);

		const FGameplayTag CostTag = FGameplayTag::RequestGameplayTag(TEXT("Data.Skill.Cost"));
		CostSpecHandle.Data->SetSetByCallerMagnitude(CostTag, -EnergyCostPerSecond);

		ASC->ApplyGameplayEffectSpecToSelf(*CostSpecHandle.Data.Get());
	}

	// 라인트레이스 후 피격 대상에 데미지 적용
	FHitResult Hit;
	FVector TraceStart, TraceEnd;

	
	if (K2_HasAuthority())
	{
		if (DoLineTrace(CurrentActorInfo, Hit, TraceStart, TraceEnd) && Hit.GetActor())
		{
			Hit.bBlockingHit = false;
			AActor* AvatarActor = GetAvatarActorFromActorInfo();
			if (!AvatarActor) return true;

			const FVector TraceDir = (TraceEnd - TraceStart).GetSafeNormal();

			const float BaseDamageLocal = PlayerAttrSet->GetSkillDamage() * SkillDamageMultiplier;
			const float FinalDamage = BaseDamageLocal * DamageMultiplier;

			UE_LOG(LogTemp, Warning, TEXT("BunnySkillFour Damage: %f (Multiplier: %.2f)"), FinalDamage, DamageMultiplier);
			UGameplayStatics::ApplyPointDamage(Hit.GetActor(), FinalDamage, TraceDir, Hit, AvatarActor->GetInstigatorController(), AvatarActor, UDamageType::StaticClass());
		}
	}
	
	return true;
}