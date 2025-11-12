// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ActivateAbilityByTag.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFD_API UBTTask_ActivateAbilityByTag : public UBTTaskNode
{
	GENERATED_BODY()

	UBTTask_ActivateAbilityByTag();

	UPROPERTY()
	FGameplayTag ActivateAbilityTag;
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
