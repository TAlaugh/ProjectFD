// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AnimNotify/AN_BossMeleeAttack.h"

#include "GameFramework/Character.h"
#include "Item/Projectile/FDBossMeleeAttack.h"


void UAN_BossMeleeAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (!MeshComp || !MeleeAttackClass) return;

	ACharacter* OwnerChar = Cast<ACharacter>(MeshComp->GetOwner());
	if (!OwnerChar) return;

	if (!OwnerChar->HasAuthority())
	{
		return;
	}

	FVector MeleeSocketLocation = MeshComp->GetSocketLocation(MeleeSocketName);

	MeleeSocketLocation.Z += 10.f;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	AFDBossMeleeAttack* MeleeAttack = OwnerChar->GetWorld()->SpawnActor<AFDBossMeleeAttack>(
			MeleeAttackClass,
			MeleeSocketLocation,
			OwnerChar->GetActorRotation(),
			SpawnParams
		);
}
