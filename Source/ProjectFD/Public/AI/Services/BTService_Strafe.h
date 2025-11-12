// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "BTService_Strafe.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFD_API UBTService_Strafe : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_Strafe();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
protected:
	UPROPERTY(EditAnywhere, Category = "BlackBoard_Key_Set")
	FBlackboardKeySelector StrafeLocation;
	
};
