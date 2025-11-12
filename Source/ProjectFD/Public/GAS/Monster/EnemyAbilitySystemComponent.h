// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/FDAbilitySystemComponent.h"
#include "EnemyAbilitySystemComponent.generated.h"

class AFDMonsterCharacter;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UEnemyAbilitySystemComponent : public UFDAbilitySystemComponent
{
	GENERATED_BODY()

public:
	// Base
	void OnAbilityInputPressed(const FGameplayTag& InInputTag);
	void OnAbilityInputReleased(const FGameplayTag& InInputTag);

	UFUNCTION(BlueprintCallable)
	bool TryActivateAbilityByTag(FGameplayTag AbilityTagToActivates);
};
