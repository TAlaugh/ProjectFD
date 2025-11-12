// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_PlasmaNet.generated.h"

class AFDBossNormalLaser;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UANS_PlasmaNet : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser")
	FName MuzzleSocketName = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser")
	FName DirSocketName = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser")
	float BasicLength = 2500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser")
	float Damage = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser")
	float Radius = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser")
	float AngleStep = 0.f;

	UPROPERTY(EditAnywhere, Category = "Laser")
	TSubclassOf<AFDBossNormalLaser> LaserEffectActorClass;

protected:
	UPROPERTY()
	TArray<AFDBossNormalLaser*> LaserEffectActors;

	UPROPERTY()
	TArray<AActor*> DamagedPlayers;
	
	UPROPERTY()
	TArray<FVector> Directions;

	void CalculateDirections(USkeletalMeshComponent* MeshComp);

	void ProcessHits(const TArray<FHitResult>& HitResults, AActor* OwnerActor);
	
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
