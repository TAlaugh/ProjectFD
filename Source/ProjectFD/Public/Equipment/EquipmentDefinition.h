// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "EquipmentDefinition.generated.h"

class AEquipmentActor;
class UEquipmentInstance;

USTRUCT()
struct FEquipmentActorToSpawn
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<AEquipmentActor> EquipmentClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	FName AttachName = FName();

	UPROPERTY(EditDefaultsOnly)
	FName UnarmedAttachName = FName();
};

UCLASS(BlueprintType, Blueprintable)
class PROJECTFD_API UEquipmentDefinition : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Info", meta=(Categories="Item.Equipment"))
	FGameplayTag ItemTag;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Info", meta=(Categories="Equipment.Slot"))
	FGameplayTag SlotTag;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Info")
	FGameplayTag RarityTag;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Info")
	FGameplayTag AmmoType;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Info")
	TSubclassOf<UEquipmentInstance> InstanceType;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Stats")
	int32 MinPossibleStats = 0;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Stats")
	int32 MaxPossibleStats = 1;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Stats", meta=(Categories="StatEffect"))
	FGameplayTag BasicStatRoll;
	
	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Stats", meta=(Categories="StatEffect"))
	FGameplayTag PossibleStatRoll;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Actors")
	TArray<FEquipmentActorToSpawn> ActorsToSpawn;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|AnimLayer")
	TSubclassOf<UAnimInstance> CharacterAnimLayer;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Abilities", meta=(Categories="Equipment.Abilities"))
	FGameplayTagContainer PossibleAbilityRolls;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Abilities")
	float ProbabilityToRollAbility = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Abilities")
	bool bForceAbilityRoll = true;
	
};
