// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/WeaponAnimInstance.h"

#include "Character/Player/FDPlayerCharacter.h"
#include "Equipment/EquipmentActor.h"


void UWeaponAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerActor = Cast<AEquipmentActor>(GetOwningActor());
}

void UWeaponAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
}

void UWeaponAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	if (OwnerActor)
	{
		bIsEquipped = OwnerActor->GetIsEquipped();
	}
	
	// if (bIsEquipped)
	// {
	// 	bShouldFire = OwnerActor->GetShouldFire();
	// 	bIsReloading = OwnerActor->GetIsReloading();
	// }
}
