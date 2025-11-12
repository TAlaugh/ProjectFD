// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "LightningBeamCue.generated.h"

class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class PROJECTFD_API ALightningBeamCue : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()
	
public:

private:
	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UNiagaraSystem* BeamEffect;
};
