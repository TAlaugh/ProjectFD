// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/Player/HangOn/HangOnGrabAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GAS/FDGameplayStatic.h"


UHangOnGrabAbility::UHangOnGrabAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ActivationRequiredTags.AddTag(UFDGameplayStatic::GetOwnerHangOnTag());
}

void UHangOnGrabAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayHangOnGrabMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, HangOnGrabMontage);
		PlayHangOnGrabMontageTask->OnCompleted.AddDynamic(this, &UHangOnGrabAbility::K2_EndAbility);
		PlayHangOnGrabMontageTask->OnBlendOut.AddDynamic(this, &UHangOnGrabAbility::K2_EndAbility);
		PlayHangOnGrabMontageTask->OnInterrupted.AddDynamic(this, &UHangOnGrabAbility::K2_EndAbility);
		PlayHangOnGrabMontageTask->OnCancelled.AddDynamic(this, &UHangOnGrabAbility::K2_EndAbility);
		PlayHangOnGrabMontageTask->ReadyForActivation();
	}
}

void UHangOnGrabAbility::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}
