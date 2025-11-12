// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorator/BTDecorator_CanUseAbilityByTag.h"

#include "AIController.h"
#include "Character/Monster/FDMonsterCharacter.h"


UBTDecorator_CanUseAbilityByTag::UBTDecorator_CanUseAbilityByTag()
{
	NodeName = "Can_Use_Ability_By_Tag";
}

bool UBTDecorator_CanUseAbilityByTag::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory) const
{
	if (AAIController* AIC = OwnerComp.GetAIOwner())
	{
		if (AFDMonsterCharacter* Enemy = Cast<AFDMonsterCharacter>(AIC->GetPawn()))
		{
			UAbilitySystemComponent* ASC = Enemy->GetEnemyAbilitySystemComponent();
			if (ASC)
			{
				// Tag가 없으면 실행 가능
				return !ASC->HasMatchingGameplayTag(CooldownTag);
			}
		}
	}
	return false;
}
