// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DA_FDGameplayAbilities.generated.h"

enum class EFDAbilityInputID : uint8;
class UGameplayAbility;
class UGameplayEffect;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UDA_FDGameplayAbilities : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// 초기세팅 게임이펙트
	UPROPERTY(EditDefaultsOnly, Category= "GE")
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;

	// DBNO 게임이펙트
	UPROPERTY(EditDefaultsOnly, Category= "GE")
	TSubclassOf<UGameplayEffect> DBNOEffect;

	// Dead 게임이펙트
	UPROPERTY(EditDefaultsOnly, Category= "GE")
	TSubclassOf<UGameplayEffect> DeadEffect;
	
	// 초기세팅 어빌리티
	UPROPERTY(EditDefaultsOnly, Category = "GA")
	TArray<TSubclassOf<UGameplayAbility>> InitializedAbilities;

	// 특수 어빌리티
	UPROPERTY(EditDefaultsOnly, Category = "GA")
	TMap<EFDAbilityInputID, TSubclassOf<UGameplayAbility>> SkillAbilities;

	// 기본 어빌리티
	UPROPERTY(EditDefaultsOnly, Category = "GA")
	TMap<EFDAbilityInputID, TSubclassOf<UGameplayAbility>> BaseAbilities;
	
	
};
