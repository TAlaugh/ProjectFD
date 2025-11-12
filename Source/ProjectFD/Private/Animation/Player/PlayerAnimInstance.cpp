// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Player/PlayerAnimInstance.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Character/Player/FDPlayerCharacter.h"


void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (OwnerCharacter)
	{
		ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerCharacter);

		AFDPlayerCharacter* PlayerCharacter = Cast<AFDPlayerCharacter>(OwnerCharacter);
		if (PlayerCharacter)
		{
			bIsSprint = PlayerCharacter->GetSprinting();
			bIsJumping = PlayerCharacter->GetJumping();
		}
		if (!ASC)
		{
			ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerCharacter);
		}
		if (ASC)
		{
			bIsCombatTag = ASC->HasMatchingGameplayTag(UFDGameplayStatic::GetOwnerCombatTag());
			bIsDBNO = ASC->HasMatchingGameplayTag(UFDGameplayStatic::GetDBNOStatusTag());
		}
	}
	if (OwnerMovementComponent)
	{
		bIsJumping = OwnerMovementComponent->IsFalling();
	}
}

void UPlayerAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	if (Speed > 0)
	{
		IdleElapsedTime = 0.f;
		bShouldEnterRelaxState = false;
	}
	else
	{
		IdleElapsedTime += DeltaSeconds;
		bShouldEnterRelaxState = (IdleElapsedTime >= EnterRelaxStateThresold);
	}
}
