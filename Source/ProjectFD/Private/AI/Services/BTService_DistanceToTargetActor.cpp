// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/BTService_DistanceToTargetActor.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTService_DistanceToTargetActor::UBTService_DistanceToTargetActor()
{
	NodeName = "Distance_To_Target_Actor";

	INIT_SERVICE_NODE_NOTIFY_FLAGS();
	
	Interval = 0.1f;
	RandomDeviation = 0.05f;

	InTargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, InTargetActorKey), AActor::StaticClass());
}

void UBTService_DistanceToTargetActor::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                                float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AAIController* AIC = OwnerComp.GetAIOwner();
	APawn* ControlledPawn = AIC->GetPawn();
	if (!BB || !ControlledPawn)
	{
		return;
	}
	
	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(InTargetActorKey.SelectedKeyName));
	if (!TargetActor)
	{
		return;
	}
	
	float Distance = TargetActor->GetDistanceTo(ControlledPawn);

	BB->SetValueAsFloat(OutDistToTargetKey.SelectedKeyName, Distance);
}
