// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/AnimNoitfyState/ANS_PlasmaNet.h"

#include "Character/Player/FDPlayerCharacter.h"
#include "Item/Projectile/FDBossNormalLaser.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

void UANS_PlasmaNet::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	DamagedPlayers.Empty();
	LaserEffectActors.Empty();
	
	if (!MeshComp)
	{
		return;
	}
	
	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor || !OwnerActor->HasAuthority())
	{
		return;
	}

	CalculateDirections(MeshComp);
	
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

	for (int32 i = 0; i < Directions.Num(); ++i)
	{
		AFDBossNormalLaser* Laser = MeshComp->GetWorld()->SpawnActor<AFDBossNormalLaser>(
			LaserEffectActorClass,
			MuzzleLocation,
			Directions[i].Rotation(),
			Params
		);

		if (Laser)
		{
			LaserEffectActors.Add(Laser);
		}
	}
}

void UANS_PlasmaNet::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	if (!MeshComp)
	{
		return;
	}

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor || !OwnerActor->HasAuthority())
	{
		return;
	}

	const FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
	
	CalculateDirections(MeshComp);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerActor);

	for (int32 i = 0; i < Directions.Num(); ++i)
	{
		FVector End = MuzzleLocation + Directions[i] * BasicLength;

		TArray<FHitResult> HitResults;
		bool bHit = MeshComp->GetWorld()->SweepMultiByChannel(
			HitResults,
			MuzzleLocation,
			End,
			FQuat::Identity,
			ECC_Visibility,
			FCollisionShape::MakeSphere(Radius),
			Params
		);

		if (LaserEffectActors.IsValidIndex(i) && LaserEffectActors[i])
		{
			LaserEffectActors[i]->SetActorLocation(MuzzleLocation);
			LaserEffectActors[i]->NetMulticast_UpdateTargetLocation(End);
		}

		if (bHit)
		{
			ProcessHits(HitResults, OwnerActor);
		}

		// DrawDebugLine(MeshComp->GetWorld(), MuzzleLocation, End, FColor::Cyan, false, 0.05f, 0, Radius);
	}
}

void UANS_PlasmaNet::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	DamagedPlayers.Empty();

	for (AFDBossNormalLaser* Laser : LaserEffectActors)
	{
		if (Laser)
		{
			Laser->Destroy();
		}
	}
	
	LaserEffectActors.Empty();
}

void UANS_PlasmaNet::CalculateDirections(USkeletalMeshComponent* MeshComp)
{
	if (!MeshComp)
	{
		return;
	}

	const FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
	const FVector DirLocation = MeshComp->GetSocketLocation(DirSocketName);
	const FVector ForwardDir = (DirLocation - MuzzleLocation).GetSafeNormal();

	Directions.Empty();
	Directions.Add(ForwardDir);

	for (int32 i = 1; i <= 3; ++i)
	{
		const FRotator RotLeft(0.f, -AngleStep * i, 0.f);
		const FRotator RotRight(0.f, AngleStep * i, 0.f);

		Directions.Add(RotLeft.RotateVector(ForwardDir));
		Directions.Add(RotRight.RotateVector(ForwardDir));
	}
}

void UANS_PlasmaNet::ProcessHits(const TArray<FHitResult>& HitResults, AActor* OwnerActor)
{
	for (const FHitResult& Hit : HitResults)
	{
		if (AFDPlayerCharacter* Player = Cast<AFDPlayerCharacter>(Hit.GetActor()))
		{
			if (!DamagedPlayers.Contains(Player))
			{
				DamagedPlayers.Add(Player);
				UGameplayStatics::ApplyDamage(Player, Damage, OwnerActor->GetInstigatorController(), OwnerActor, nullptr);
			}
		}
	}
}