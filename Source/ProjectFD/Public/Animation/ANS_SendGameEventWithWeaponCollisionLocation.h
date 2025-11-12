// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_SendGameEventWithWeaponCollisionLocation.generated.h"

class AWeaponBase;
enum class EWeaponType : uint8;

USTRUCT()
struct FWeaponCollisionLocation
{
	GENERATED_BODY()
public:
	FVector StartLocation;
	FVector EndLocation;
};

UENUM(BlueprintType)
enum class EWeaponEquipType : uint8
{
	Equipped,
	None,
};
/**
 * 
 */
UCLASS()
class PROJECTFD_API UANS_SendGameEventWithWeaponCollisionLocation : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Event")
	FGameplayTag EventTag;

	UPROPERTY(EditAnywhere, Category = "Event")
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, Category = "Event")
	FName StartSocketName = FName("Start");
	
	UPROPERTY(EditAnywhere, Category = "Event")
	FName EndSocketName = FName("End");
	
	UPROPERTY(EditAnywhere, Category = "Event")
	EWeaponEquipType EquipType;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

private:
	AWeaponBase* Weapon;

	UPROPERTY()
	TArray<FWeaponCollisionLocation> WeaponCollisionLocations;
};
