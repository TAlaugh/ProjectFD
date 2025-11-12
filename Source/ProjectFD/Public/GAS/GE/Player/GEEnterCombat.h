// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GEEnterCombat.generated.h"

class UAssetTagsGameplayEffectComponent;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UGEEnterCombat : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UGEEnterCombat();
	UPROPERTY()
	UAssetTagsGameplayEffectComponent* Component;

};
