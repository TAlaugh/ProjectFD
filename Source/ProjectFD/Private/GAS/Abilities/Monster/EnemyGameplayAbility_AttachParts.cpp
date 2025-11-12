// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/Monster/EnemyGameplayAbility_AttachParts.h"

#include "Character/Monster/Boss/FDBossCharacter.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "FunctionLibrary/FDFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"


UEnemyGameplayAbility_AttachParts::UEnemyGameplayAbility_AttachParts()
{
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::OwnedTagPresent;
	TriggerData.TriggerTag = UFDGameplayStatic::PlayerAttachParts();
	AbilityTriggers.Add(TriggerData);
}

void UEnemyGameplayAbility_AttachParts::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		return;

	AActor* OwnerActor = ActorInfo->OwnerActor.Get();
	if (!OwnerActor->HasAuthority())
	{
		return;
	}
	
	if (OwnerActor)
	{ 
		if (!UFDFunctionLibrary::NativeDoesActorHaveTag(OwnerActor, UFDGameplayStatic::BerserkGageBlock()))
		{
			UFDFunctionLibrary::AddGameplayTagToActorIfNone(OwnerActor, UFDGameplayStatic::BerserkGageBlock());
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
