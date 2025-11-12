// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EquipmentInstance.generated.h"

struct FEquipmentActorToSpawn;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UEquipmentInstance : public UObject
{
	GENERATED_BODY()
	
public:

	UEquipmentInstance();

	virtual void OnEquip(AActor* Owner, bool bIsActive);
	virtual void OnUnequip();

	void SpawnEquipmentActors(const TArray<FEquipmentActorToSpawn>& ActorsToSpawn, bool bIsActiveSlot, bool bIsSameUnarmedLocation);
	void UnarmEquipmentActors(TArray<FEquipmentActorToSpawn> ActorsToSpawn);
	void DestroyEquipmentActors();
	void SetWeaponVisibility(bool InIsVisible);

	float GetFireTime();

	UPROPERTY()
	bool bIsActiveWeapon = false;

private:

	UPROPERTY()
	TArray<AActor*> SpawnedActors;

	ACharacter* GetCharacter();

	float FireTime = 0.f;
	
};
