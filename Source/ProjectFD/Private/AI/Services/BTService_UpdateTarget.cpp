// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/BTService_UpdateTarget.h"

#include "AIController.h"
#include "Character/Monster/FDMonsterCharacter.h"


UBTService_UpdateTarget::UBTService_UpdateTarget()
{
	NodeName = "Update_Target";
	Interval = 1.f;
}

void UBTService_UpdateTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AAIController* AIC = OwnerComp.GetAIOwner();
	APawn* ControlledPawn = AIC->GetPawn();
	if (!BB || !ControlledPawn)
	{
		return;
	}
	
	if (AIC)
	{
		if (AFDMonsterCharacter* EnemyCharacter = Cast<AFDMonsterCharacter>(ControlledPawn))
		{
			EnemyCharacter->UpdateNearestTarget();
		}
	}
}
