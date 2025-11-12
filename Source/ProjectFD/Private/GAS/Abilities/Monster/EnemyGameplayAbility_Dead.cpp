// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/Monster/EnemyGameplayAbility_Dead.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/Monster/FDMonsterCharacter.h"


void UEnemyGameplayAbility_Dead::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                 const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                 const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		return;

	AActor* OwnerActor = ActorInfo->OwnerActor.Get();
	AFDMonsterCharacter* MC = Cast<AFDMonsterCharacter>(OwnerActor);

	if (PlayToMontage)
	{
		UAbilityTask_PlayMontageAndWait* PlayMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, PlayToMontage);
		PlayMontage->OnBlendOut.AddDynamic(this, &UEnemyGameplayAbility_Dead::MonsterDestroy);
		PlayMontage->OnCancelled.AddDynamic(this, &UEnemyGameplayAbility_Dead::MonsterDestroy);
		PlayMontage->OnCompleted.AddDynamic(this, &UEnemyGameplayAbility_Dead::MonsterDestroy);
		PlayMontage->OnInterrupted.AddDynamic(this, &UEnemyGameplayAbility_Dead::MonsterDestroy);
		PlayMontage->ReadyForActivation();
	}
}

void UEnemyGameplayAbility_Dead::MonsterDestroy()
{
	AActor* OwnerActor = GetAvatarActorFromActorInfo();
	if (!OwnerActor || !OwnerActor->HasAuthority())
	{
		return;
	}

	AFDMonsterCharacter* Monster = Cast<AFDMonsterCharacter>(OwnerActor);
	if (!Monster)
	{
		return;
	}

	Monster->Destroy();

	K2_EndAbility();
}
