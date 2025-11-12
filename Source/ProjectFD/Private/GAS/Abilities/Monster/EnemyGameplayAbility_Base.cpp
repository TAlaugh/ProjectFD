// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/Monster/EnemyGameplayAbility_Base.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Character/Monster/Boss/FDBossCharacter.h"
#include "FunctionLibrary/FDFunctionLibrary.h"
#include "GAS/FDAbilitySystemComponent.h"
#include "GAS/FDGameplayStatic.h"


UEnemyGameplayAbility_Base::UEnemyGameplayAbility_Base()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UEnemyGameplayAbility_Base::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                 const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                 const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		return;

	AActor* OwnerActor = ActorInfo->OwnerActor.Get();
	AFDMonsterCharacter* MC = Cast<AFDMonsterCharacter>(OwnerActor);
	
	if (!MC->bIsPunch && PlayToMontage)
	{
		UAbilityTask_PlayMontageAndWait* PlayMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, PlayToMontage);
		PlayMontage->OnBlendOut.AddDynamic(this, &UEnemyGameplayAbility_Base::OnMontageEndWait);
		PlayMontage->OnCancelled.AddDynamic(this, &UEnemyGameplayAbility_Base::OnMontageEndWait);
		PlayMontage->OnCompleted.AddDynamic(this, &UEnemyGameplayAbility_Base::OnMontageEndWait);
		PlayMontage->OnInterrupted.AddDynamic(this, &UEnemyGameplayAbility_Base::OnMontageEndWait);
		PlayMontage->ReadyForActivation();
	}
	else if (MC->bIsPunch && DestroyAttachPartMontage)
	{
		UAbilityTask_PlayMontageAndWait* PlayMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, DestroyAttachPartMontage);
		PlayMontage->OnBlendOut.AddDynamic(this, &UEnemyGameplayAbility_Base::OnMontageEndWait);
		PlayMontage->OnCancelled.AddDynamic(this, &UEnemyGameplayAbility_Base::OnMontageEndWait);
		PlayMontage->OnCompleted.AddDynamic(this, &UEnemyGameplayAbility_Base::OnMontageEndWait);
		PlayMontage->OnInterrupted.AddDynamic(this, &UEnemyGameplayAbility_Base::OnMontageEndWait);
		PlayMontage->ReadyForActivation();
	}

	if (CooldownGameplayEffect)
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(
				CooldownGameplayEffect, GetAbilityLevel());

			if (SpecHandle.IsValid())
			{
				ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
			}
		}
	}
}

void UEnemyGameplayAbility_Base::OnMontageEndWait()
{
	UAbilityTask_WaitDelay* WaitTask = UAbilityTask_WaitDelay::WaitDelay(this, 2.f);
	WaitTask->OnFinish.AddDynamic(this, &UEnemyGameplayAbility_Base::OnWaitFinished);
	WaitTask->ReadyForActivation();
}

void UEnemyGameplayAbility_Base::OnWaitFinished()
{
	K2_EndAbility();
}
