// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"
/**
 * 
 */
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None	UMETA(DisplayName = "None"),
	MG		UMETA(DisplayName = "MachineGun"),
	AR		UMETA(DisplayName = "AssaultRifle"),
	SMG		UMETA(DisplayName = "SubMachineGun"),
	SR		UMETA(DisplayName = "SniperRifle"),
	SG		UMETA(DisplayName = "ShotGun"),
	LNC		UMETA(DisplayName = "RocketLauncher"),
};

UCLASS()
class PROJECTFD_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	USkeletalMeshComponent* WeaponMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hand", meta = (ExposeOnSpawn = "true"))
	EWeaponType WeaponType = EWeaponType::MG;

	USkeletalMeshComponent* GetMesh() const;
	
};
