// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/BTService_Strafe.h"

#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "FunctionLibrary/FDFunctionLibrary.h"
#include "GAS/FDGameplayStatic.h"

UBTService_Strafe::UBTService_Strafe()
{
	NodeName = "MoveTo_StrafeLocation";
	
	INIT_SERVICE_NODE_NOTIFY_FLAGS();
}

void UBTService_Strafe::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AAIController* AIC = OwnerComp.GetAIOwner();
	APawn* ControlledPawn = AIC->GetPawn();
	if (!BB || !ControlledPawn)
	{
		return;
	}

	if (UAbilitySystemComponent* ASC = ControlledPawn->FindComponentByClass<UAbilitySystemComponent>())
	{
		if (ASC->HasMatchingGameplayTag(UFDGameplayStatic::GetBerserkStartTag()) || ASC->HasMatchingGameplayTag(UFDGameplayStatic::GetNormalStartTag()))
		{
			return;
		}
	}
	
	FVector Dest = BB->GetValueAsVector(StrafeLocation.SelectedKeyName);

	AIC->MoveToLocation(Dest);
	
}
