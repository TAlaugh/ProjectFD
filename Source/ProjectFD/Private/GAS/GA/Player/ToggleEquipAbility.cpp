// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/Player/ToggleEquipAbility.h"

#include "GAS/FDGameplayStatic.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"


UToggleEquipAbility::UToggleEquipAbility()
{
}

void UToggleEquipAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASC = GetFDAbilitySystemComponentFromActorInfo();
	PlayerCharacter = Cast<AFDPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!PlayerCharacter)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (ASC->HasMatchingGameplayTag(UFDGameplayStatic::GetOwnerCombatTag()))
	{
		OnUnEquipMontage();
	}
	else if (!ASC->HasMatchingGameplayTag(UFDGameplayStatic::GetOwnerCombatTag()))
	{
		OnEquipMontage();
	}
}

void UToggleEquipAbility::OnEquipMontage()
{
	if (!EquipMontage)
	{
		OnEndAbility();
		return;
	}

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("EquipMontage"), EquipMontage, 1.0f);
	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnEndAbility);
	MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnEndAbility);
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnEndAbility);
	MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnEndAbility);
	MontageTask->ReadyForActivation();
	OnGameplayTask();
}

void UToggleEquipAbility::OnUnEquipMontage()
{
	if (!UnEquipMontage)
	{
		OnEndAbility();
		return;
	}

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("UnEquipMontage"), UnEquipMontage, 1.0f);
	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnEndAbility);
	MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnEndAbility);
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnEndAbility);
	MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnEndAbility);
	MontageTask->ReadyForActivation();
	OnGameplayTask();
}

void UToggleEquipAbility::OnGameplayTask()
{
	if (!ASC) return;
	
	FGameplayTag EventTag;

	if (ASC->HasMatchingGameplayTag(UFDGameplayStatic::GetOwnerCombatTag()))
	{
		EventTag = FGameplayTag::RequestGameplayTag(TEXT("Event.UnEquip"));
		GameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EventTag, nullptr, false);
		GameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::OnUnEquipGameplayEvent);
	}
	else if (!ASC->HasMatchingGameplayTag(UFDGameplayStatic::GetOwnerCombatTag()))
	{
		EventTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Equip"));
		GameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EventTag, nullptr, false);
		GameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::OnEquipGameplayEvent);
	}

	if (GameplayEventTask.IsValid())
	{
		GameplayEventTask->ReadyForActivation();
	}
	else
	{
		OnEndAbility();
	}
}

void UToggleEquipAbility::OnEndAbility()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UToggleEquipAbility::OnEquipGameplayEvent(FGameplayEventData Payload)
{
	if (!ASC)
	{
		OnEndAbility();
	}

	if (CurrentActorInfo && CurrentActorInfo->IsNetAuthority())
	{
		if (GEGrantEquipTag)
		{
			const FGameplayEffectContextHandle Ctx = ASC->MakeEffectContext();
			const FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(GEGrantEquipTag, 1.f, Ctx);
			if (Spec.IsValid())
			{
				ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
			}
		}
	}
	else
	{
		Server_SetEquipped(true);
	}

	OnEndAbility();
}

void UToggleEquipAbility::OnUnEquipGameplayEvent(FGameplayEventData Payload)
{
	if (!ASC)
	{
		OnEndAbility();
	}

	if (CurrentActorInfo && CurrentActorInfo->IsNetAuthority())
	{
		FGameplayTagContainer Granted;
		Granted.AddTag(UFDGameplayStatic::GetOwnerCombatTag());
		// 전투 태그를 '부여'하는 활성 GE를 전부 제거 => 태그 자동 해제
		ASC->RemoveActiveEffectsWithGrantedTags(Granted);
	}
	else
	{
		Server_SetEquipped(false);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UToggleEquipAbility::Server_SetEquipped_Implementation(bool bEquipped)
{
	if (!ASC) return;

	if (bEquipped)
	{
		if (GEGrantEquipTag)
		{
			const FGameplayEffectContextHandle Ctx = ASC->MakeEffectContext();
			const FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(GEGrantEquipTag, 1.f, Ctx);
			if (Spec.IsValid())
			{
				ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
			}
		}
	}
	else
	{
		FGameplayTagContainer Granted;
		Granted.AddTag(UFDGameplayStatic::GetOwnerCombatTag());
		ASC->RemoveActiveEffectsWithGrantedTags(Granted);
	}
}
