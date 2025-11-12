// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_ExecutionCallAttack.generated.h"

class AFDBossECAttack;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UAN_ExecutionCallAttack : public UAnimNotify
{
	GENERATED_BODY()
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
	UPROPERTY(EditAnywhere)
	float Damage;

	UPROPERTY(EditAnywhere)
	float Radius;

	UPROPERTY(EditAnywhere, Category = "ExecutionCall")
	TSubclassOf<AFDBossECAttack> AttackEffectActorClass;
	
	UPROPERTY()
	FVector BossLocation;
	
};
