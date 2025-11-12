// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Projectile/FDBossMeleeAttack.h"

#include "NiagaraFunctionLibrary.h"

AFDBossMeleeAttack::AFDBossMeleeAttack()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

// Called when the game starts or when spawned
void AFDBossMeleeAttack::BeginPlay()
{
	Super::BeginPlay();

	SpawnAttackEffect();

	FTimerHandle DestroyTimer;
	GetWorld()->GetTimerManager().SetTimer(DestroyTimer,
		[this]()
		{
			Destroy();
		}, 3.f, false);
}

void AFDBossMeleeAttack::SpawnAttackEffect()
{
	if (AttackEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			AttackEffect,
			GetActorLocation(),
			GetActorRotation(),
			FVector(1.f)
		);
	}
}