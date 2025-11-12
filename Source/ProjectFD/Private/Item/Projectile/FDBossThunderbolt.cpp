// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Projectile/FDBossThunderbolt.h"

#include "NiagaraFunctionLibrary.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "Compression/lz4.h"
#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFDBossThunderbolt::AFDBossThunderbolt()
{
	bReplicates = true;
}

void AFDBossThunderbolt::BeginPlay()
{
	Super::BeginPlay();

	SpawnWarningEffect();

	FTimerHandle SpawnThunderbolt;
	GetWorld()->GetTimerManager().SetTimer(
		SpawnThunderbolt,
		[this]()
		{
			SpawnThunder();
			UGameplayStatics::PlaySound2D(GetWorld(), ThunderSound);
		},
		0.5f,
		false
	);
}

void AFDBossThunderbolt::SpawnThunder()
{
	if (HasAuthority())
	{
		UWorld* World = GetWorld();
		if (!World)
		{
			return;
		}
	
		TArray<FOverlapResult> Overlaps;
		FCollisionQueryParams Params;
	
		bool bHit = World->OverlapMultiByChannel(
			Overlaps,
			GetActorLocation(),
			FQuat::Identity,
			ECC_Visibility,
			FCollisionShape::MakeSphere(ThunderboltRadius),
			Params
		);
	
		if (bHit)
		{
			for (auto& Result : Overlaps)
			{
				AFDPlayerCharacter* Player = Cast<AFDPlayerCharacter>(Result.GetActor());
				if (Player)
				{
					UGameplayStatics::ApplyDamage(Player, Damage, nullptr, this, nullptr);
				}
			}
		}
		// DrawDebugSphere(World, GetActorLocation(), ThunderboltRadius, 16, FColor::Red, false, 2.f);
	}
	
	if (ThunderEffect)
	{
		SpawnThunderEffect();
	}

	FTimerHandle DestroyHandle;
	GetWorld()->GetTimerManager().SetTimer(
		DestroyHandle,
		[this]()
		{
			Destroy();
		},
		1.f,
		false
		);

}

void AFDBossThunderbolt::SpawnWarningEffect()
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

void AFDBossThunderbolt::SpawnThunderEffect()
{
	if (ThunderEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ThunderEffect,
			GetActorLocation(),
			FRotator::ZeroRotator,
			FVector(1.f)
		);
	}
}