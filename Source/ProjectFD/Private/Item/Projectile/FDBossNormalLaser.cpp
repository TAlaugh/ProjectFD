// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Projectile/FDBossNormalLaser.h"

#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AFDBossNormalLaser::AFDBossNormalLaser()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	LaserEffectComponent = CreateDefaultSubobject<UNiagaraComponent>("LaserNiagara");
	SetRootComponent(LaserEffectComponent);
}

void AFDBossNormalLaser::NetMulticast_PBOUpdateTargetLocation_Implementation(const FVector& TargetLocation,
	const FVector& MuzzleLocation)
{
	if (LaserEffectComponent)
	{
		float LaserLength = (TargetLocation - MuzzleLocation).Size();
		LaserEffectComponent->SetFloatParameter("LaserLength", LaserLength / 150.f);
	}
}

void AFDBossNormalLaser::NetMulticast_UpdateTargetLocation_Implementation(const FVector& TargetLocation)
{
	if (LaserEffectComponent)
	{
		LaserEffectComponent->SetVectorParameter("Beam End", TargetLocation);
	}
	if (HitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitParticle,
			TargetLocation,
			FRotator::ZeroRotator,
			1.f
		);
	}
}

