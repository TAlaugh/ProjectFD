// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Projectile/FDBossECSpawn.h"

#include "NiagaraFunctionLibrary.h"


AFDBossECSpawn::AFDBossECSpawn()
{
	// PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AFDBossECSpawn::BeginPlay()
{
	Super::BeginPlay();

	SpawnWarningEffect();

	FTimerHandle DestroyTimer;
	GetWorld()->GetTimerManager().SetTimer(DestroyTimer,
		[this]()
		{
			Destroy();
		}, 5.f, false);
}

void AFDBossECSpawn::SpawnWarningEffect()
{
	if (WarningEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			WarningEffect,
			GetActorLocation(),
			FRotator::ZeroRotator,
			FVector(1.f)
		);
	}
}


