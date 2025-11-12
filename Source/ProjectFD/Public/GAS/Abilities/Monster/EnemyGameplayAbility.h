// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/FDGameplayAbility.h"
#include "EnemyGameplayAbility.generated.h"

class UEnemyCombatComponent;
class AFDMonsterCharacter;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UEnemyGameplayAbility : public UFDGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Enemy|Ability")
	AFDMonsterCharacter* GetEnemyCharacterFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Enemy|Ability")
	UEnemyCombatComponent* GetEnemyCombatComponentFromActorInfo();
	
private:
	TWeakObjectPtr<AFDMonsterCharacter> CachedEnemyCharacter;

};
