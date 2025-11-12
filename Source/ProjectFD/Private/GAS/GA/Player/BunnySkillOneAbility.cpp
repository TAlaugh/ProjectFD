// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/GA/Player/BunnySkillOneAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "GAS/FDGameplayStatic.h"
#include "GAS/Player/PlayerAttributeSet.h"
#include "Kismet/GameplayStatics.h"


UBunnySkillOneAbility::UBunnySkillOneAbility()
{
	ActivationRequiredTags.AddTag(UFDGameplayStatic::GetOwnerCombatTag());
	CooldownTag = FGameplayTag::RequestGameplayTag(FName("Data.Skill.Cooldown.1"));
}

bool UBunnySkillOneAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
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

void UBunnySkillOneAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = GetFDAbilitySystemComponentFromActorInfo();
	const UPlayerAttributeSet* PlayerAttrSet = ASC->GetSet<UPlayerAttributeSet>();
	

	// 비용 계산 & 자원 체크
	const float BunnySkillOneCost = CostAmount * PlayerAttrSet->GetSkillCost();
	if (PlayerAttrSet->GetSpecialResource() < BunnySkillOneCost )
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
	const float BunnySkillOneCooldown = SkillCooldown * PlayerAttrSet->GetSkillCooldown();
	CooldownSpecHandle.Data->SetDuration(BunnySkillOneCooldown, true);
	CooldownSpecHandle.Data->DynamicGrantedTags.AddTag(CooldownTag);
	
	
	// SetByCaller를 이용해 CostAmount 설정
	const FGameplayTag CostTag = FGameplayTag::RequestGameplayTag(TEXT("Data.Skill.Cost"));
	CostSpecHandle.Data->SetSetByCallerMagnitude(CostTag, -BunnySkillOneCost);
	
	
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

	// 몽타주/이벤트 태스크 시작
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlaySkillMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, SkillMontage);
		PlaySkillMontageTask->OnBlendOut.AddDynamic(this, &UBunnySkillOneAbility::K2_EndAbility);
		PlaySkillMontageTask->OnCompleted.AddDynamic(this, &UBunnySkillOneAbility::K2_EndAbility);
		PlaySkillMontageTask->OnInterrupted.AddDynamic(this, &UBunnySkillOneAbility::K2_EndAbility);
		PlaySkillMontageTask->OnCancelled.AddDynamic(this, &UBunnySkillOneAbility::K2_EndAbility);
		PlaySkillMontageTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitSkillEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag(FName("Event.BunnySkill.One")), nullptr, false, true);
		WaitSkillEventTask->EventReceived.AddDynamic(this, &UBunnySkillOneAbility::StartTargetSweep);
		WaitSkillEventTask->ReadyForActivation();
	}	
}

void UBunnySkillOneAbility::StartTargetSweep(FGameplayEventData EventData)
{
	AActor* OwnerActor = GetAvatarActorFromActorInfo();
	UAbilitySystemComponent* ASC = GetFDAbilitySystemComponentFromActorInfo();
	const UPlayerAttributeSet* PlayerAttrSet = ASC->GetSet<UPlayerAttributeSet>();
	const float BunnySkillOneRadius = TargetSweepSphereRadius * PlayerAttrSet->GetSkillRadius();

	// 현재 위치를 시작\(&목표\) 위치로 명시적으로 지정
	FGameplayAbilityTargetDataHandle CurrentTargetData;
	{
		FGameplayAbilityTargetData_LocationInfo* LocData = new FGameplayAbilityTargetData_LocationInfo();

		// 스윕 시작점: 소스 = 현재 소유 액터 트랜스폼
		LocData->SourceLocation.LocationType = EGameplayAbilityTargetingLocationType::ActorTransform;
		LocData->SourceLocation.SourceActor = OwnerActor;
		
		// 스윕 목표점(중심): 현재 액터 트랜스폼
		LocData->TargetLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
		LocData->TargetLocation.LiteralTransform = OwnerActor->GetActorTransform();

		CurrentTargetData.Add(LocData);
	}

	// 사운드 재생
	
	TArray<FHitResult> TargetHitResults = GetHitResultFromSphereSweepLocationTargetData(CurrentTargetData, BunnySkillOneRadius, TargetAmount, ShouldDebugDraw(), ETeamAttitude::Hostile, false);
	if (K2_HasAuthority())
	{
		AFDPlayerCharacter* PC = Cast<AFDPlayerCharacter>(GetAvatarActorFromActorInfo());
		FVector StartLocation = OwnerActor->FindComponentByClass<USkeletalMeshComponent>()->GetSocketLocation(SocketName);
		PC->Multicast_SpawnSkillNiagaraEffectAtLocation(SkillOneSpawnSphereNiagaraSystem, StartLocation, StartLocation);
		PC->Multicast_PlaySkillSoundAtLocation(SkillOneSound, StartLocation);
		for (FHitResult HitResult : TargetHitResults)
		{
			HitResult.bBlockingHit = false;

			UWorld* World = GetWorld();
			FVector EndLocation = HitResult.ImpactPoint;

			PC->Multicast_SpawnSkillNiagaraEffectAtLocation(SkillOneNiagaraSystem, StartLocation, EndLocation);

			FHitResult LineTraceHit;
			FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SkillThreeLineTrace), true);
			QueryParams.AddIgnoredActor(OwnerActor);

			const bool bHit = OwnerActor->GetWorld()->LineTraceSingleByChannel(
				LineTraceHit,
				StartLocation,
				EndLocation,
				ECC_Visibility,
				QueryParams
			);
			if (ShouldDebugDraw())
			{
				DrawDebugLine(World, StartLocation, EndLocation, bHit ? FColor::Red : FColor::Green, false, DrawDebugDuration, 0, 2.0f);
				if (bHit)
				{
					DrawDebugPoint(World, LineTraceHit.ImpactPoint, 10.f, FColor::Yellow, false, DrawDebugDuration);
				}
			}
			
			float BunnySkillOneDamage = (PlayerAttrSet->GetSkillDamage() * SkillDamageMultiplier) + (((PlayerAttrSet->GetSkillDamage() * 0.15f ) * (static_cast<int>(PlayerAttrSet->GetSpecialResource()) / 30)));
			bool bIsCriticalHit = FMath::FRand() <= PlayerAttrSet->GetSkillCriticalChance();
			
			if (bIsCriticalHit)
			{
				BunnySkillOneDamage *= PlayerAttrSet->GetSkillCriticalDamage();
			}
			UE_LOG(LogTemp, Warning, TEXT("BunnySkillOneDamage: %f"), BunnySkillOneDamage);
			UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), BunnySkillOneDamage, (EndLocation - StartLocation).GetSafeNormal(), HitResult, OwnerActor->GetInstigatorController(), OwnerActor, UDamageType::StaticClass());
		}
	}
}