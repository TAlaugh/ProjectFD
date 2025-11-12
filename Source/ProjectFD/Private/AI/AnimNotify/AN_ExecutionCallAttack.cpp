// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AnimNotify/AN_ExecutionCallAttack.h"

#include "Character/Monster/Boss/FDBossCharacter.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "Engine/OverlapResult.h"
#include "Item/Projectile/FDBossECAttack.h"
#include "Kismet/GameplayStatics.h"


void UAN_ExecutionCallAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;
	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor || !OwnerActor->HasAuthority()) return;

	UWorld* World = OwnerActor->GetWorld();
	if (!World) return;

	AFDBossCharacter* Boss = Cast<AFDBossCharacter>(OwnerActor);
	
	if (Boss)
	{
		BossLocation = Boss->ExecutionCall_DamageVector;
	}

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerActor);

	bool bHit = World->OverlapMultiByChannel(
		Overlaps,
		BossLocation,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(Radius),
		Params
	);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AFDBossECAttack* AttackEffect = Boss->GetWorld()->SpawnActor<AFDBossECAttack>(
			AttackEffectActorClass,
			Boss->ExecutionCall_DamageVector - FVector(0.f,0.f,87.f),
			FRotator::ZeroRotator,
			SpawnParameters
		);
	
	if (bHit)
	{
		for (auto& Result : Overlaps)
		{
			AFDPlayerCharacter* Player = Cast<AFDPlayerCharacter>(Result.GetActor());
			if (Player)
			{
				UGameplayStatics::ApplyDamage(Player, Damage, OwnerActor->GetInstigatorController(), OwnerActor, nullptr);
			}
		}
	}

	// DrawDebugSphere(World, BossLocation, Radius, 16, FColor::Red, false, 2.f);
}
