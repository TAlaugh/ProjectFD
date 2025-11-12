// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/EquipmentInstance.h"

#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Equipment/EquipmentActor.h"
#include "Equipment/EquipmentDefinition.h"
#include "GameFramework/Character.h"
#include "Interfaces/WeaponInterface.h"


UEquipmentInstance::UEquipmentInstance()
{
	bIsActiveWeapon = false;
}

void UEquipmentInstance::OnEquip(AActor* Owner, bool bIsActive)
{
	// Caching Weapon to Onwing Character
	if (bIsActive)
	{
		if (not SpawnedActors.IsEmpty())
		{
			if (IWeaponInterface* Interface = Cast<IWeaponInterface>(Owner))
			{
				FireTime = Cast<AEquipmentActor>(SpawnedActors[0])->GetFireMontageLength();
				Interface->SetWeaponActor(Cast<AEquipmentActor>(SpawnedActors[0]));
			}
		}
	}
}

void UEquipmentInstance::OnUnequip()
{
	if (ACharacter* Owner = GetCharacter())
	{
		if (not SpawnedActors.IsEmpty())
		{
			if (IWeaponInterface* Interface = Cast<IWeaponInterface>(Owner))
			{
				Interface->SetWeaponActor(nullptr);
			}
		}
	}
}

void UEquipmentInstance::SpawnEquipmentActors(const TArray<FEquipmentActorToSpawn>& ActorsToSpawn, bool bIsActiveSlot, bool bIsSameUnarmedLocation)
{
	if (ACharacter* OwningCharacter = GetCharacter())
	{
		// if Already Spawned
		if (!SpawnedActors.IsEmpty())
		{
			Cast<AEquipmentActor>(SpawnedActors[0])->SetWeaponVisibility(true);
			Cast<AEquipmentActor>(SpawnedActors[0])->SetIsEquipped(true);
			SpawnedActors[0]->AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, ActorsToSpawn[0].AttachName);
			bIsActiveWeapon = true;
			OnEquip(OwningCharacter, true);
			return;
		}
		
		FStreamableManager& Manager = UAssetManager::GetStreamableManager();
		TWeakObjectPtr<UEquipmentInstance> WeakThis(this);
		
		for (const FEquipmentActorToSpawn& ActorToSpawn : ActorsToSpawn)
		{
			if (IsValid(ActorToSpawn.EquipmentClass.Get()))
			{
				AEquipmentActor* NewActor = GetWorld()->SpawnActorDeferred<AEquipmentActor>(ActorToSpawn.EquipmentClass.Get(),
					FTransform::Identity, OwningCharacter);

				NewActor->FinishSpawning(FTransform::Identity);
				NewActor->SetReplicates(true);
				if (bIsActiveSlot)
				{
					bIsActiveWeapon = true;
					NewActor->AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, ActorToSpawn.AttachName);
					NewActor->SetIsEquipped(true);
					// OnEquip(OwningCharacter, true);
				}
				else
				{
					bIsActiveWeapon = false;
					NewActor->AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, ActorToSpawn.UnarmedAttachName);
					if (bIsSameUnarmedLocation)
					{
						NewActor->SetWeaponVisibility(false);
					}
				}
				FireTime = NewActor->GetFireMontageLength();
				SpawnedActors.Emplace(NewActor);
				if (bIsActiveSlot)
				{
					OnEquip(OwningCharacter, true);
				}
			}
			else
			{
				Manager.RequestAsyncLoad(ActorToSpawn.EquipmentClass.ToSoftObjectPath(),
					[WeakThis, ActorToSpawn, OwningCharacter, bIsActiveSlot, bIsSameUnarmedLocation]
					{
						if (UWorld* OurWorld = WeakThis->GetWorld())
						{
							AEquipmentActor* NewActor = OurWorld->SpawnActorDeferred<AEquipmentActor>(ActorToSpawn.EquipmentClass.Get(),
						FTransform::Identity, OwningCharacter);

							NewActor->FinishSpawning(FTransform::Identity);
							NewActor->SetReplicates(true);
							if (bIsActiveSlot)
							{
								WeakThis->bIsActiveWeapon = true;
								NewActor->AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, ActorToSpawn.AttachName);
								NewActor->SetIsEquipped(true);
								// WeakThis->OnEquip(OwningCharacter, true);
							}
							else
							{
								WeakThis->bIsActiveWeapon = false;
								NewActor->AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, ActorToSpawn.UnarmedAttachName);
								if (bIsSameUnarmedLocation)
								{
									NewActor->SetWeaponVisibility(false);
								}
							}
							WeakThis->FireTime = NewActor->GetFireMontageLength();
							WeakThis->SpawnedActors.Emplace(NewActor);
							if (bIsActiveSlot)
							{
								WeakThis->OnEquip(OwningCharacter, true);
							}
						}
					});
			}
		}
	}
}

void UEquipmentInstance::UnarmEquipmentActors(TArray<FEquipmentActorToSpawn> ActorsToSpawn)
{
	bIsActiveWeapon = false;
	if (not SpawnedActors.IsEmpty() && GetCharacter())
	{
		Cast<AEquipmentActor>(SpawnedActors[0])->SetIsEquipped(false);
		SpawnedActors[0]->AttachToComponent(GetCharacter()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, ActorsToSpawn[0].UnarmedAttachName);
	}
}

void UEquipmentInstance::DestroyEquipmentActors()
{
	bIsActiveWeapon = false;
	for (AActor* Actor : SpawnedActors)
	{
		Actor->Destroy();
	}
}

void UEquipmentInstance::SetWeaponVisibility(bool InIsVisible)
{
	for (AActor* Actor : SpawnedActors)
	{
		Cast<AEquipmentActor>(Actor)->SetWeaponVisibility(InIsVisible);
	}
}

float UEquipmentInstance::GetFireTime()
{
	return FireTime;
}

ACharacter* UEquipmentInstance::GetCharacter()
{
	if (GetOuter())
	{
		return Cast<ACharacter>(GetOuter());
	}

	return nullptr;
}
