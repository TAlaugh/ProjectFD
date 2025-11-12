// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "ModuleDefinition.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class PROJECTFD_API UModuleDefinition : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Info", meta=(Categories="Item.Equipment.Module"))
	FGameplayTag ItemTag;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Info")
	FGameplayTag RarityTag;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Info", meta=(Categories = "Module.SocketType"))
	FGameplayTag SocketTypeTag = FGameplayTag::EmptyTag;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Info", meta=(Categories = "Module.ChangeType"))
	FGameplayTag ChangeTypeTag = FGameplayTag::EmptyTag;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Info")
	int32 Capacity = 0;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Info")
	int32 EnchantMaxCount = 0;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Ability", meta=(Categories="Ability"))
	FGameplayTagContainer AbilitiesToGrant;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Ability", meta=(Categories="Ability"))
	FGameplayTagContainer AbilitiesToRemove;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Stats", meta=(Categories="StatEffect"))
	FGameplayTag BasicStatRoll;
	
};
