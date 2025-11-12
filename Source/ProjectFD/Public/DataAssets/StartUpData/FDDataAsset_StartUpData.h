// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FDDataAsset_StartUpData.generated.h"

class UGameplayEffect;
class UFDAbilitySystemComponent;
class UFDGameplayAbility;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UFDDataAsset_StartUpData : public UDataAsset
{
	GENERATED_BODY()

public:
	virtual void GiveToAbilitySystemComponent(UFDAbilitySystemComponent* InASCToGive, int32 ApplyLevel = 1);
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UFDGameplayAbility>> ActivateOnGivenAbilities;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UFDGameplayAbility>> ReactiveAbilities;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayEffect>> StartUpGameplayEffect;
	
	void GrantAbilities(const TArray<TSubclassOf<UFDGameplayAbility>>& InAbilitiesToGive,UFDAbilitySystemComponent* InASCToGive, int32 ApplyLevel = 1);
};
