// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CanUseAbilityByTag.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFD_API UBTDecorator_CanUseAbilityByTag : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTDecorator_CanUseAbilityByTag();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

public:
	UPROPERTY(EditAnywhere, Category = "Ability")
	FGameplayTag CooldownTag;
};
