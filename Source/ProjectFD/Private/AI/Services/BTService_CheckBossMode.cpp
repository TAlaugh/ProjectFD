// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/BTService_CheckBossMode.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/Monster/FDMonsterCharacter.h"
#include "Character/Monster/Boss/FDBossCharacter.h"
#include "GAS/Monster/EnemyAttributeSet.h"


UBTService_CheckBossMode::UBTService_CheckBossMode()
{
	NodeName = "Check_Boss_Berserk";

	INIT_SERVICE_NODE_NOTIFY_FLAGS();
	
}

void UBTService_CheckBossMode::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AAIController* AIC = OwnerComp.GetAIOwner();
	APawn* ControlledPawn = AIC->GetPawn();
	if (!BB || !ControlledPawn)
	{
		return;
	}
	
	AFDMonsterCharacter* EnemyCharacter = Cast<AFDMonsterCharacter>(ControlledPawn);
	if (!EnemyCharacter)
	{
		return;
	}
	UEnemyAttributeSet* EnemyAttributeSet = EnemyCharacter->GetEnemyAttributeSet();
	if (!EnemyAttributeSet)
	{
		return;
	}

	if (AFDBossCharacter* Boss = Cast<AFDBossCharacter>(EnemyCharacter))
	{
		BB->SetValueAsBool(bIsBossBerserk.SelectedKeyName, Boss->GetBerserkMode());
	}
}
