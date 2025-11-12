// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CheckBossMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFD_API UBTService_CheckBossMode : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_CheckBossMode();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
protected:
	// true 면 Berserk / false 면 Normal
	UPROPERTY()
	FBlackboardKeySelector bIsBossBerserk;

	
};
