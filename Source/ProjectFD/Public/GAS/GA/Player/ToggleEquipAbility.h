// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/FDGameplayAbility.h"
#include "ToggleEquipAbility.generated.h"

class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_PlayMontageAndWait;
class AFDPlayerCharacter;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UToggleEquipAbility : public UFDGameplayAbility
{
	GENERATED_BODY()
	
public:
	UToggleEquipAbility();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, Category = "GE")
	TSubclassOf<UGameplayEffect> GEGrantEquipTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* EquipMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* UnEquipMontage;
	
	void OnEquipMontage();
	void OnUnEquipMontage();
	void OnGameplayTask();
	UFUNCTION()
	void OnEndAbility();

	UFUNCTION()
	void OnEquipGameplayEvent(FGameplayEventData Payload);
	UFUNCTION()
	void OnUnEquipGameplayEvent(FGameplayEventData Payload);

	UFUNCTION(Server, Reliable)
	void Server_SetEquipped(bool bEquipped);

private:
	UPROPERTY()
	TObjectPtr<AFDPlayerCharacter> PlayerCharacter;
	UPROPERTY()
	UAbilitySystemComponent* ASC;

	FActiveGameplayEffectHandle CurrentStateGEHandle;

	// 테스크 관련 변수 (데이터 관리를 위해 포인터로 선언
	TWeakObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;
	TWeakObjectPtr<UAbilityTask_WaitGameplayEvent> GameplayEventTask;
	
};
