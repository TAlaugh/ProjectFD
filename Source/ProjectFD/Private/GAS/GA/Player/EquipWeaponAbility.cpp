// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/Player/EquipWeaponAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/Player/FDPlayerCharacter.h"


UEquipWeaponAbility::UEquipWeaponAbility()
{
}

void UEquipWeaponAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle,ActorInfo,ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo,true, false);
		return;
	}

	AFDPlayerCharacter* AvatarPC = ActorInfo ? Cast<AFDPlayerCharacter>(ActorInfo->AvatarActor.Get()) : nullptr;
	PC = AvatarPC;

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayEquipMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, EquipMontage);
		PlayEquipMontageTask->OnBlendOut.AddDynamic(this, &UEquipWeaponAbility::K2_EndAbility);
		PlayEquipMontageTask->OnCompleted.AddDynamic(this, &UEquipWeaponAbility::K2_EndAbility);
		PlayEquipMontageTask->OnInterrupted.AddDynamic(this, &UEquipWeaponAbility::K2_EndAbility);
		PlayEquipMontageTask->OnCancelled.AddDynamic(this, &UEquipWeaponAbility::K2_EndAbility);
		PlayEquipMontageTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitSkillEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag(FName("Event.EquipWeapon")), nullptr, true, true);
		WaitSkillEventTask->EventReceived.AddDynamic(this, &UEquipWeaponAbility::ChangeWeapon);
		WaitSkillEventTask->ReadyForActivation();
	}
}

void UEquipWeaponAbility::ChangeWeapon(FGameplayEventData EventData)
{
	if (!IsActive())
	{
		return;
	}

	AFDPlayerCharacter* AvatarPC = PC.Get();
	AvatarPC->ChangeWeaponBySlot(EquipNumber);
	K2_EndAbility();
}
