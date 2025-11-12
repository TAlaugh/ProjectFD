// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_RemoveTag.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFD_API UAN_RemoveTag : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
	UPROPERTY(EditAnywhere)
	FGameplayTag RemoveTag;
	
	UPROPERTY(EditAnywhere)
	FGameplayTag GrantTag;

	UPROPERTY(EditAnywhere)
	bool CallEndBerserk = false;
};
