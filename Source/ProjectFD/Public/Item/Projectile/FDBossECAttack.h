// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FDBossECAttack.generated.h"

class UNiagaraSystem;

UCLASS()
class PROJECTFD_API AFDBossECAttack : public AActor
{
	GENERATED_BODY()
	
public:	
	AFDBossECAttack();

protected:
	virtual void BeginPlay() override;

	void SpawnAttackEffect();
	
	UPROPERTY(EditAnywhere, Category = "ExecutionCall")
	UNiagaraSystem* AttackEffect;

	UPROPERTY(EditAnywhere, Category = "ExecutionCall")
	UParticleSystem* ThunderEffect;
};
