// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/DropItem/DropItemBase.h"
#include "DropItemBase_Equipment.generated.h"

UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
	Tier1,
	Tier2,
	Tier3
};

/**
 * 
 */
UCLASS()
class PROJECTFD_API ADropItemBase_Equipment : public ADropItemBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEquipmentType EquipmentType;
	
protected:
	virtual void HandlePickUp(AFDPlayerCharacter* Player) override;
	
	void EquipmentTierOne(AFDPlayerCharacter* Player);
	void EquipmentTierTwo(AFDPlayerCharacter* Player);
	void EquipmentTierThree(AFDPlayerCharacter* Player);
};
