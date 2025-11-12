// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "EquipmentStatEffects.generated.h"

struct FEquipmentStatEffectGroup;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UEquipmentStatEffects : public UDataAsset
{
	GENERATED_BODY()
	
public:

	// UPROPERTY(EditDefaultsOnly)
	// TMap<FGameplayTag, TObjectPtr<UDataTable>> MasterStatMap;
	
	UPROPERTY(EditDefaultsOnly, Category=Stats, meta=(Categories="StatEffect.Weapon"))
	TMap<FGameplayTag, TObjectPtr<UDataTable>> WeaponStatMap;

	UPROPERTY(EditDefaultsOnly, Category=Stats, meta=(Categories="StatEffect.ExComponent"))
	TMap<FGameplayTag, TObjectPtr<UDataTable>> ExComponentStatMap;

	UPROPERTY(EditDefaultsOnly, Category=Stats, meta=(Categories="Item.Equipment.Module"))
	TMap<FGameplayTag, TObjectPtr<UDataTable>> ModuleStatMap;

	UPROPERTY(EditDefaultsOnly, Category=Abilities, meta=(Categories="Item.Equipment.Weapon"))
	TMap<FGameplayTag, TObjectPtr<UDataTable>> WeaponAbilityMap;

	UPROPERTY(EditDefaultsOnly, Category=Abilities, meta=(Categories="Item.Equipment.ExComponent"))
	TMap<FGameplayTag, TObjectPtr<UDataTable>> ExComponentAbilityMap;

	UPROPERTY(EditDefaultsOnly, Category=Abilities, meta=(Categories="Item.Equipment.Module"))
	TMap<FGameplayTag, TObjectPtr<UDataTable>> ModuleAbilityMap;
	
	
};
