// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimInstance.h"
#include "GAS/FDAbilitySystemComponent.h"
#include "GAS/FDGameplayStatic.h"
#include "FDAnimInstance.generated.h"

class UFDAbilitySystemComponent;
//enum ECombatState : uint8;
class UCharacterMovementComponent;
/**
 * 
 */

UCLASS()
class PROJECTFD_API UFDAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY()
	ACharacter* OwnerCharacter;
	UPROPERTY()
	UCharacterMovementComponent* OwnerMovementComponent;
	float Speed;


public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	FORCEINLINE float GetSpeed() const { return Speed; }


protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	float LocomotionDirection;
	
};
