// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_PBO.generated.h"

class AFDBossNormalLaser;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UANS_PBO : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser")
	FName MuzzleSocketName = "";
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser")
	float ExtraLength = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser")
	float TraceInterval = 0.33f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser")
	float Damage = 0.f;

	UPROPERTY(EditAnywhere, Category = "Laser")
	TSubclassOf<AFDBossNormalLaser> LaserEffectActorClass;

	UPROPERTY()
	AFDBossNormalLaser* LaserEffectActor;
	
protected:
	FVector PrevEnd;

	FVector TargetLocation;

	float TimerAccumulator = 0.f;
	
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
};
