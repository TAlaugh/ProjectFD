// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/FDAnimInstance.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Character/Player/FDPlayerCharacter.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KismetAnimationLibrary.h"
#include "FunctionLibrary/FDFunctionLibrary.h"


void UFDAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	// 애니메이션에서 사용할 캐릭터, 캐릭터 무브먼트 캐시
	OwnerCharacter = Cast<AFDCharacter>(TryGetPawnOwner());
	if (OwnerCharacter)
	{
		OwnerMovementComponent = OwnerCharacter->GetCharacterMovement();
	}
}

void UFDAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (OwnerCharacter)
	{
		Speed = OwnerCharacter->GetVelocity().Length();
	}
}

void UFDAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	// ---
	if (!OwnerCharacter || !OwnerMovementComponent)
	{
		return;
	}
	
	LocomotionDirection = UKismetAnimationLibrary::CalculateDirection(OwnerCharacter->GetVelocity(), OwnerCharacter->GetActorRotation());
}


