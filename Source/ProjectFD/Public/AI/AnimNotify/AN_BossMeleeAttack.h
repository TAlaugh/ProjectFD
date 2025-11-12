// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_BossMeleeAttack.generated.h"

class AFDBossMeleeAttack;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UAN_BossMeleeAttack : public UAnimNotify
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Melee")
	TSubclassOf<AFDBossMeleeAttack> MeleeAttackClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Melee")
	FName MeleeSocketName = "";

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
	
};
