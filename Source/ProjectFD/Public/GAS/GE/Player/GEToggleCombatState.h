// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GEToggleCombatState.generated.h"


class UPlayerAbilitySystemComponent;
class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UGEToggleCombatState : public UGameplayEffect
{
	GENERATED_BODY()
	
public:
	UGEToggleCombatState();
	FInheritedTagContainer InheritableOwnedTags;

	UPROPERTY()
	UPlayerAbilitySystemComponent* ASC;
};
