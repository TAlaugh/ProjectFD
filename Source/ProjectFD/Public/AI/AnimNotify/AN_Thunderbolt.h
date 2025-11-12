// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_Thunderbolt.generated.h"

class AFDBossThunderbolt;
class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UAN_Thunderbolt : public UAnimNotify
{
	GENERATED_BODY()

public:
	FTimerHandle ThunderboltTimeHandle;

	UPROPERTY(EditAnywhere, Category = "Thunderbolt")
	UClass* ThunderActorClass;
	
	UPROPERTY(EditAnywhere, Category = "Thunderbolt")
	float ThunderboltDuration = 15.f;
	
	UPROPERTY(EditAnywhere, Category = "Thunderbolt")
	float ThunderboltInterval = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Thunderbolt")
	float MaxRadius = 0.f;

	UPROPERTY(EditAnywhere, Category = "Thunderbolt")
	float TraceHeight = 0.f;

private:
	float ThunderboltElapsed = 0.f;
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	UFUNCTION()
	void FindPlayer(AActor* OwnerActor);
};
