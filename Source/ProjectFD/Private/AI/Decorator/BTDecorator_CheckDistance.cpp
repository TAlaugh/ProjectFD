// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorator/BTDecorator_CheckDistance.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Elements/Framework/TypedElementQueryBuilder.h"


UBTDecorator_CheckDistance::UBTDecorator_CheckDistance()
{
	NodeName = "Check_Distance_To_TargetActor";
	
}

bool UBTDecorator_CheckDistance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return false;
	}

	const float Distance = BB->GetValueAsFloat(DistanceKey.SelectedKeyName);

	if (bCheckLessEqualDist)
	{
		return Distance <= ThresholdDistance;
	}
	else
	{
		return Distance >= ThresholdDistance;
	}
}
