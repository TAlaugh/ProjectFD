// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Projectile/FDBossECAttack.h"

#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AFDBossECAttack::AFDBossECAttack()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

// Called when the game starts or when spawned
void AFDBossECAttack::BeginPlay()
{
	Super::BeginPlay();

	SpawnAttackEffect();
	
	FTimerHandle DestroyTimer;
	GetWorld()->GetTimerManager().SetTimer(DestroyTimer,
		[this]()
		{
			Destroy();
		}, 2.f, false);
}

void AFDBossECAttack::SpawnAttackEffect()
{
	if (AttackEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			AttackEffect,
			GetActorLocation(),
			FRotator::ZeroRotator,
			FVector(1.f)
		);
	}

	if (ThunderEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ThunderEffect,
			GetActorLocation(),
			FRotator::ZeroRotator,
			FVector(6.f)
		);
	}
}
