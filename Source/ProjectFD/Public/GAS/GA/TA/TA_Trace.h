// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TA_Trace.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFD_API ATA_Trace : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
	
public:
	ATA_Trace();

	virtual void StartTargeting(UGameplayAbility* Ability) override;
	virtual void ConfirmTargetingAndContinue() override;
	void SetShowDebug(bool bInShowDebug) { bShowDebug = bInShowDebug; }
	
protected:
	virtual FGameplayAbilityTargetDataHandle MakeTargetData() const;

	bool bShowDebug = false;


	
};
