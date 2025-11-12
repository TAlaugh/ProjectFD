// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/Player/MeleeAttackAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "GAS/Player/PlayerAttributeSet.h"
#include "Kismet/GameplayStatics.h"


UMeleeAttackAbility::UMeleeAttackAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UMeleeAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AFDPlayerCharacter* PC = CastChecked<AFDPlayerCharacter>(ActorInfo->AvatarActor.Get());

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayPunchTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, PunchActionMontage, 1.0f, NAME_None, false);
		PlayPunchTask->OnCompleted.AddDynamic(this, &UMeleeAttackAbility::K2_EndAbility);
		PlayPunchTask->OnInterrupted.AddDynamic(this, &UMeleeAttackAbility::K2_EndAbility);
		PlayPunchTask->OnCancelled.AddDynamic(this, &UMeleeAttackAbility::K2_EndAbility);
		PlayPunchTask->OnBlendOut.AddDynamic(this, &UMeleeAttackAbility::K2_EndAbility);
		PlayPunchTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag(FName("Event.MeleeAttack")), nullptr, false);
		WaitEventTask->EventReceived.AddDynamic(this, &UMeleeAttackAbility::OnMeleeAttackEvent);
		WaitEventTask->ReadyForActivation();
	}
}

void UMeleeAttackAbility::OnMeleeAttackEvent(FGameplayEventData EventData)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	AFDPlayerCharacter* PC = Cast<AFDPlayerCharacter>(GetAvatarActorFromActorInfo());
	// 현재 위치를 시작\(&목표\) 위치로 명시적으로 지정
	FGameplayAbilityTargetDataHandle CurrentTargetData;
	{
		FGameplayAbilityTargetData_LocationInfo* LocData = new FGameplayAbilityTargetData_LocationInfo();

		// // 스윕 시작점: 소스 = 현재 소유 액터 트랜스폼
		LocData->SourceLocation.LocationType = EGameplayAbilityTargetingLocationType::ActorTransform;
		LocData->SourceLocation.SourceActor = AvatarActor;
		const FVector Origin = AvatarActor->GetActorLocation();
		const FVector Forward = AvatarActor->GetActorForwardVector();
		const FTransform NewTargetTransform(AvatarActor->GetActorRotation(), (Origin + Forward * 100.0f), FVector::OneVector);
		
		// 스윕 목표점(중심): 현재 액터 트랜스폼
		LocData->TargetLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
		LocData->TargetLocation.LiteralTransform = NewTargetTransform;

		CurrentTargetData.Add(LocData);
	}
	
	UAbilitySystemComponent* ASC = GetFDAbilitySystemComponentFromActorInfo();
	const UPlayerAttributeSet* PlayerAttrSet = ASC->GetSet<UPlayerAttributeSet>();
	float MeleeAttackDamage = PlayerAttrSet->GetSkillDamage();
	
	TArray<FHitResult> TargetHitResults = GetHitResultFromSphereSweepLocationTargetData(CurrentTargetData, MeleeAttackRange, 2, ShouldDebugDraw(), ETeamAttitude::Hostile, false);

	for (FHitResult HitResult : TargetHitResults)
	{
		HitResult.bBlockingHit = false;
		
		FVector StartLocation = HitResult.TraceStart;
		FVector EndLocation = HitResult.ImpactPoint;

		PC->Multicast_SpawnSkillNiagaraEffectAtLocation(MeleeAttackEffect, EndLocation, StartLocation);


		bool bIsCriticalHit = FMath::FRand() <= PlayerAttrSet->GetSkillCriticalChance();

		if (bIsCriticalHit)
		{
			MeleeAttackDamage *= PlayerAttrSet->GetSkillCriticalDamage();
		}
		UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), MeleeAttackDamage, HitResult.ImpactPoint, HitResult, ASC->GetOwnerActor()->GetInstigatorController(), ASC->GetOwnerActor(), UDamageType::StaticClass());
	}
}
