// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_AttackHitCheck.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFD_API UAN_AttackHitCheck : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UAN_AttackHitCheck();

protected:
	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference);

	UPROPERTY(EditAnywhere)
	FGameplayTag TriggerGameplayTag;
	
	
};
