// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/DropItem/DropItemBase.h"
#include "DropItemBase_Rune.generated.h"

UENUM(BlueprintType)
enum class ERuneType : uint8
{
	Tier1,
	Tier2,
	Tier3,
	Tier4
};

/**
 * 
 */
UCLASS()
class PROJECTFD_API ADropItemBase_Rune : public ADropItemBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERuneType RuneType;
	
protected:
	virtual void HandlePickUp(AFDPlayerCharacter* Player) override;
	
	void RuneTierOne(AFDPlayerCharacter* Player);
	void RuneTierTwo(AFDPlayerCharacter* Player);
	void RuneTierThree(AFDPlayerCharacter* Player);
	void RuneTierFour(AFDPlayerCharacter* Player);
};
