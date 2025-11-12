// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CheckDistance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFD_API UBTDecorator_CheckDistance : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTDecorator_CheckDistance();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
	UPROPERTY(EditAnywhere, Category = "BlackBoard_Key_Set")
	FBlackboardKeySelector DistanceKey;
	
	UPROPERTY(EditAnywhere, Category = "Condition")
	float ThresholdDistance = 800.f;

	UPROPERTY(EditAnywhere, Category = "Condition")
	bool bCheckLessEqualDist = true;
};
