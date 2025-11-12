// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GAS/FDAttributeSet.h"
#include "EnemyAttributeSet.generated.h"

/**
 * 
 */

UCLASS()
class PROJECTFD_API UEnemyAttributeSet : public UFDAttributeSet
{
	GENERATED_BODY()
	
public:
	UEnemyAttributeSet();
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BerserkGage)
	FGameplayAttributeData BerserkGage;
	ATTRIBUTE_ACCESSORS(UEnemyAttributeSet, BerserkGage);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxBerserkGage)
	FGameplayAttributeData MaxBerserkGage;
	ATTRIBUTE_ACCESSORS(UEnemyAttributeSet, MaxBerserkGage);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attack)
	FGameplayAttributeData Attack;
	ATTRIBUTE_ACCESSORS(UEnemyAttributeSet, Attack);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Defence)
	FGameplayAttributeData Defence;
	ATTRIBUTE_ACCESSORS(UEnemyAttributeSet, Defence);
	
public:
	virtual void OnRep_Health(const FGameplayAttributeData& OldValue) override;
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldValue) override;
	virtual void OnRep_Shield(const FGameplayAttributeData& OldValue) override;
	virtual void OnRep_MaxShield(const FGameplayAttributeData& OldValue) override;
	
	UFUNCTION()
	void OnRep_BerserkGage(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxBerserkGage(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Attack(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Defence(const FGameplayAttributeData& OldValue);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
