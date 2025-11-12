// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WeaponAnimInstance.generated.h"

class AEquipmentActor;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UWeaponAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly, Category="Status")
	bool bIsEquipped = false;
	UPROPERTY(BlueprintReadOnly, Category="Status")
	bool bShouldFire = false;
	UPROPERTY(BlueprintReadOnly, Category="Status")
	bool bIsReloading = false;

private:
	
	UPROPERTY()
	AEquipmentActor* OwnerActor;
	
	
};
