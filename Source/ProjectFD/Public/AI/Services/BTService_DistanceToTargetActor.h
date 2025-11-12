// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_DistanceToTargetActor.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFD_API UBTService_DistanceToTargetActor : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_DistanceToTargetActor();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "BlackBoard_Key_Set")
	FBlackboardKeySelector InTargetActorKey;
	
	UPROPERTY(EditAnywhere, Category = "BlackBoard_Key_Set")
	FBlackboardKeySelector OutDistToTargetKey;
	
};
