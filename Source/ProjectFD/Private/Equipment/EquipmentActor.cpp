// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/EquipmentActor.h"

#include "Character/Player/FDPlayerCharacter.h"
#include "GAS/Player/PlayerAttributeSet.h"
#include "Net/UnrealNetwork.h"

AEquipmentActor::AEquipmentActor()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// SetReplicates(true);
	bReplicates = true;

	RootScene = CreateDefaultSubobject<USceneComponent>("RootScene");
	SetRootComponent(RootScene);

	EquipmentMesh = CreateDefaultSubobject<USkeletalMeshComponent>("EquipmentMesh");
	EquipmentMesh->SetupAttachment(GetRootComponent());
	EquipmentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void AEquipmentActor::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);

	if (NewOwner)
	{
		WeaponFireDelegate.AddUObject(this, &AEquipmentActor::PlayFireMontage);
		WeaponReloadDelegate.AddUObject(this, &AEquipmentActor::PlayReloadMontage);
	}
	else
	{
		WeaponFireDelegate.Clear();
		WeaponReloadDelegate.Clear();
	}
}

void AEquipmentActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEquipmentActor, bIsWeaponVisible);
	DOREPLIFETIME(AEquipmentActor, bIsEquipped);
}

bool AEquipmentActor::GetIsEquipped()
{
	return bIsEquipped;
}

void AEquipmentActor::SetIsEquipped(bool InIsEquipped)
{
	this->bIsEquipped = InIsEquipped;
}

void AEquipmentActor::PlayFireMontage(bool InShouldFire)
{
	if (InShouldFire && FireMontage && not EquipmentMesh->GetAnimInstance()->Montage_IsPlaying(FireMontage))
	{
		const UPlayerAttributeSet* AttrSet = Cast<AFDPlayerCharacter>(GetOwner())->GetAbilitySystemComponent()->GetSet<UPlayerAttributeSet>();
		if (AttrSet)
		{
			EquipmentMesh->GetAnimInstance()->Montage_Play(FireMontage, AttrSet->GetFireRate());
		}
		else
		{
			EquipmentMesh->GetAnimInstance()->Montage_Play(FireMontage, 1);
		}
	}
	else if (not InShouldFire && EquipmentMesh->GetAnimInstance()->Montage_IsPlaying(FireMontage))
	{
		EquipmentMesh->GetAnimInstance()->Montage_Stop(0.2, FireMontage);
	}
}

void AEquipmentActor::PlayReloadMontage(bool InIsReloading)
{
	if (InIsReloading)
	{
		EquipmentMesh->GetAnimInstance()->Montage_Play(ReloadMontage, 1);
	}
}

float AEquipmentActor::GetFireMontageLength()
{
	if (FireMontage)
	{
		return FireMontage->GetPlayLength();
	}

	return 1.f;
}

void AEquipmentActor::SetWeaponVisibility(bool InIsVisible)
{
	if (!HasAuthority())
	{
		ServerSetVisibility(InIsVisible);
	}
	else
	{
		bIsWeaponVisible = InIsVisible;
		OnRep_SetVisibility();
	}
}

void AEquipmentActor::ServerSetVisibility_Implementation(bool InIsVisible)
{
	bIsWeaponVisible = InIsVisible;
	OnRep_SetVisibility();
}

// bool AEquipmentActor::GetShouldFire()
// {
// 	return bShouldFire;
// }
//
// void AEquipmentActor::SetShouldFire(bool bShouldFire)
// {
// 	this->bShouldFire = bShouldFire;
// }
//
// bool AEquipmentActor::GetIsReloading()
// {
// 	return bIsReloading;
// }
//
// void AEquipmentActor::SetIsReloading(bool bIsReloading)
// {
// 	this->bIsReloading = bIsReloading;
// }

void AEquipmentActor::OnRep_SetVisibility()
{
	SetActorHiddenInGame(!bIsWeaponVisible);
}

