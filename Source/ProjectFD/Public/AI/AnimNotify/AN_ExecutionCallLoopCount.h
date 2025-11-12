// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_ExecutionCallLoopCount.generated.h"

class AFDBossECSpawn;
class AFDBossCharacter;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UAN_ExecutionCallLoopCount : public UAnimNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	UPROPERTY(EditAnywhere, Category = "ExecutionCall")
	TSubclassOf<AFDBossECSpawn> WarningEffectActorClass;
	
	UPROPERTY(EditAnywhere)
	int32 TeleportPlayerCount = 4;

	UPROPERTY(EditAnywhere)
	int32 AM_EndSectionCount = 12;	
	
	UFUNCTION()
	void TeleportPlayer(AFDBossCharacter* Boss);
	
};
