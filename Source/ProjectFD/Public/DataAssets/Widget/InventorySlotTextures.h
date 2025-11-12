// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "InventorySlotTextures.generated.h"

struct FGameplayTag;
class UTexture2D;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UInventorySlotTextures : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly, Category=Weapon)
	TMap<FGameplayTag, UTexture2D*> WeaponSlot;

	UPROPERTY(EditDefaultsOnly, Category=Weapon, meta=(Categories="UI.Module"))
	TMap<FGameplayTag, UTexture2D*> ModuleSlot;

	UPROPERTY(EditDefaultsOnly, Category=Weapon)
	TMap<FGameplayTag, UTexture2D*> HudWeaponInfo;
	
	
};
