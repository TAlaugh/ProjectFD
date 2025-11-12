// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FDBossECSpawn.generated.h"

class UNiagaraSystem;

UCLASS()
class PROJECTFD_API AFDBossECSpawn : public AActor
{
	GENERATED_BODY()
	
public:	
	AFDBossECSpawn();

protected:
	virtual void BeginPlay() override;
	
	void SpawnWarningEffect();
	
	UPROPERTY(EditAnywhere, Category = "ExecutionCall")
	UNiagaraSystem* WarningEffect;
	
};
