// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/EquipmentManagerComponent.h"
#include "GAS/FDAbilitySystemComponent.h"
#include "PlayerAbilitySystemComponent.generated.h"

struct FFDEquipmentEntry;
class UDA_FDInput;
class UDA_FDGameplayAbilities;
enum class EFDAbilityInputID : uint8;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UPlayerAbilitySystemComponent : public UFDAbilitySystemComponent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "DataControl")
	UDA_FDGameplayAbilities* GAData;
	
public:
	void ApplyInitializeEffects();
	void OperatingInitializedAbilities();
	void AddEquipmentEffects(FFDEquipmentEntry* EquipmentEntry);
	void RemoveEquipmentEffects(FFDEquipmentEntry* EquipmentEntry);
	void AddEquipmentAbilities(FFDEquipmentEntry* EquipmentEntry);
	void RemoveEquipmentAbilities(FFDEquipmentEntry* EquipmentEntry);


	UPlayerAbilitySystemComponent();
	void HealthUpdated(const FOnAttributeChangeData& OnAttributeChangeData);

	void ApplyFullStatEffect();
private:
	void AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> GameplayEffect, int Level);

	FGameplayAbilitySpecHandle GrantEquipmentAbility(const FEquipmentAbilityGroup& AbilityGroup);

	FActiveGameplayEffectHandle DBNOEffectHandle;
};
