// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/FDPawnExtensionComponent.h"
#include "PawnCombatComponent.generated.h"

enum class EWeaponType : uint8;
class AWeaponBase;

/**
 * 
 */
UCLASS()
class PROJECTFD_API UPawnCombatComponent : public UFDPawnExtensionComponent
{
	GENERATED_BODY()
	
public:
	UPawnCombatComponent();

	UPROPERTY(Replicated)
	TArray<AWeaponBase*> EquippedWeapons;

	UFUNCTION(BlueprintCallable)
	void EquippedWeapon(AWeaponBase* Weapon);

	UFUNCTION(BlueprintCallable)
	AWeaponBase* GetEquippedWeapon(EWeaponType Type);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	
};
