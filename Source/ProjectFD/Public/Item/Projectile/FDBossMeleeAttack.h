// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FDBossMeleeAttack.generated.h"

class UNiagaraSystem;

UCLASS()
class PROJECTFD_API AFDBossMeleeAttack : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFDBossMeleeAttack();

protected:
	virtual void BeginPlay() override;
	
	void SpawnAttackEffect();
	
	UPROPERTY(EditAnywhere, Category = "MeleeAttack")
	UNiagaraSystem* AttackEffect;
};
