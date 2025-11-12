// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_ActivateAbilityByTag.h"

#include "AIController.h"
#include "Character/Monster/FDMonsterCharacter.h"
#include "Character/Monster/Boss/FDBossCharacter.h"
#include "GAS/Monster/EnemyAbilitySystemComponent.h"

UBTTask_ActivateAbilityByTag::UBTTask_ActivateAbilityByTag()
{
	NodeName = TEXT("Task_Activate_Ability_By_Tag");
}

EBTNodeResult::Type UBTTask_ActivateAbilityByTag::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AFDBossCharacter* Boss = Cast<AFDBossCharacter>(OwnerComp.GetOwner()))
	{
		if (UEnemyAbilitySystemComponent* EnemyASC = Cast<UEnemyAbilitySystemComponent>(Boss->GetEnemyAbilitySystemComponent()))
		{
			EnemyASC->TryActivateAbilityByTag(ActivateAbilityTag);
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}
