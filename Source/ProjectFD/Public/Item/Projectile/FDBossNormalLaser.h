// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FDBossNormalLaser.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class PROJECTFD_API AFDBossNormalLaser : public AActor
{
	GENERATED_BODY()
	
public:	
	AFDBossNormalLaser();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraComponent* LaserEffectComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* HitParticle;

public:
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_UpdateTargetLocation(const FVector& TargetLocation);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_PBOUpdateTargetLocation(const FVector& TargetLocation, const FVector& MuzzleLocation);
};
