// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/FDGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/Combat/PawnCombatComponent.h"
#include "Components/FDPawnExtensionComponent.h"
#include "GAS/FDAbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"


void UFDGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (AbilityActivationPolicy == EFDAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo && !Spec.IsActive())
		{
			ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
		}
	}
}

void UFDGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (AbilityActivationPolicy == EFDAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo)
		{
			ActorInfo->AbilitySystemComponent->ClearAbility(Handle);
		}
	}
}

UPawnCombatComponent* UFDGameplayAbility::GetPawnCombatComponentFromActorInfo() const
{
	return GetAvatarActorFromActorInfo()->FindComponentByClass<UPawnCombatComponent>();
}

UAbilitySystemComponent* UFDGameplayAbility::GetFDAbilitySystemComponentFromActorInfo() const
{
	return Cast<UFDAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent);
}

UAnimInstance* UFDGameplayAbility::GetAnimInstance() const
{
	USkeletalMeshComponent* OwningSkeletalMeshComponent = GetOwningComponentFromActorInfo();
	if (OwningSkeletalMeshComponent)
	{
		return OwningSkeletalMeshComponent->GetAnimInstance();
	}
	return nullptr;
}
