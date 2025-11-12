// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/StartUpData/FDDataAsset_StartUpData.h"
#include "FDDataAsset_EnemyStartUpData.generated.h"

class UEnemyGameplayAbility;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UFDDataAsset_EnemyStartUpData : public UFDDataAsset_StartUpData
{
	GENERATED_BODY()

public:
	virtual void GiveToAbilitySystemComponent(UFDAbilitySystemComponent* InASCToGive, int32 ApplyLevel = 1) override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray<TSubclassOf<UEnemyGameplayAbility>> EnemyCombatAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray<TSubclassOf<UEnemyGameplayAbility>> DestroyAbilities;
	
};
