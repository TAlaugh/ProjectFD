// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_SendGameplayEventToActor.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFD_API UAN_SendGameplayEventToActor : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Event")
	FGameplayTag EventTag;

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	virtual FString GetNotifyName_Implementation() const override;
};
