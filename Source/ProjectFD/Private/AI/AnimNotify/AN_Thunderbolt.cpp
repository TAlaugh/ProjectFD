// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AnimNotify/AN_Thunderbolt.h"

#include "NiagaraFunctionLibrary.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "Engine/OverlapResult.h"
#include "Item/Projectile/FDBossThunderbolt.h"
#include "Kismet/GameplayStatics.h"


void UAN_Thunderbolt::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp)
	{
		return;
	}

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor)
	{
		return;
	}
	
	if (!OwnerActor->HasAuthority())
	{
		return;
	}
	
	ThunderboltElapsed = 0.f;

	OwnerActor->GetWorldTimerManager().SetTimer(
		ThunderboltTimeHandle,
		[this, OwnerActor]()
		{
			if (!IsValid(OwnerActor))
			{
				if (UWorld* World = GEngine->GetWorldFromContextObject(OwnerActor, EGetWorldErrorMode::ReturnNull))
				{
					World->GetTimerManager().ClearTimer(ThunderboltTimeHandle);
				}
				return;
			}
			
			FindPlayer(OwnerActor);

			ThunderboltElapsed += ThunderboltInterval;
			if (ThunderboltElapsed >= ThunderboltDuration)
			{
				OwnerActor->GetWorldTimerManager().ClearTimer(ThunderboltTimeHandle);
			}
		},
		ThunderboltInterval,
		true
	);
	
}

void UAN_Thunderbolt::FindPlayer(AActor* OwnerActor)
{
	UWorld* World = OwnerActor->GetWorld();
	if (!World)
	{
		return;
	}

	TArray<AActor*> Players;
	UGameplayStatics::GetAllActorsOfClass(World, AFDPlayerCharacter::StaticClass(), Players);

	for (AActor* PlayerActor : Players)
	{
		if (!PlayerActor)
		{
			continue;
		}
		
		FVector PlayerLocation = PlayerActor->GetActorLocation();

		float RandRadius = FMath::FRandRange(0.f, MaxRadius);
		float RandAngle = FMath::FRandRange(0.f, 360.f);

		FVector2D Offset2D(
			FMath::Cos(FMath::DegreesToRadians(RandAngle)) * RandRadius,
			FMath::Sin(FMath::DegreesToRadians(RandAngle)) * RandRadius
		);

		FVector SpawnLocation = PlayerLocation + FVector(Offset2D.X, Offset2D.Y, 0.f);

		FHitResult HitResult;
		FVector Start = SpawnLocation + FVector(0.f, 0.f, TraceHeight);
		FVector End = SpawnLocation - FVector(0.f, 0.f, TraceHeight * 2);

		FCollisionQueryParams TraceParams;
		TraceParams.bTraceComplex = false;
		TraceParams.AddIgnoredActor(OwnerActor);
		TraceParams.AddIgnoredActor(PlayerActor);
		
		if (World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, TraceParams))
		{
			SpawnLocation = HitResult.Location;
		}

		// Spawn ThunderActor
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AFDBossThunderbolt* Thunder = OwnerActor->GetWorld()->SpawnActor<AFDBossThunderbolt>(
			ThunderActorClass,
			SpawnLocation,
			FRotator::ZeroRotator,
			SpawnParameters
		);
	}
}