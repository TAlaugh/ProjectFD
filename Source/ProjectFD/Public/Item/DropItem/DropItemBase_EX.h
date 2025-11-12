// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/DropItem/DropItemBase.h"
#include "DropItemBase_EX.generated.h"

UENUM(BlueprintType)
enum class EEXType : uint8
{
	Heal,
	MpHeal,
	Buff
};

/**
 * 
 */
UCLASS()
class PROJECTFD_API ADropItemBase_EX : public ADropItemBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEXType EXType;
	
protected:
	virtual void HandlePickUp(AFDPlayerCharacter* Player) override;

	void EXHpHeal(AFDPlayerCharacter* Player);
	void EXMpHeal(AFDPlayerCharacter* Player);
	void EXBuff(AFDPlayerCharacter* Player);
	
};
