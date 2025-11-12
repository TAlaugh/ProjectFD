// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_MINSpawn.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFD_API UAN_MINSpawn : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TSubclassOf<AActor> MinionClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	int32 SpawnCount = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	float SpawnRadius = 200.f;
	
	
	
};
