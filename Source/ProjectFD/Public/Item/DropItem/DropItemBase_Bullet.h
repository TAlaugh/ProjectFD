// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/DropItem/DropItemBase.h"
#include "DropItemBase_Bullet.generated.h"

UENUM(BlueprintType)
enum class EBulletType : uint8
{
	Enhanced,
	Impact,
	General,
	HighPower
};

/**
 * 
 */
UCLASS()
class PROJECTFD_API ADropItemBase_Bullet : public ADropItemBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBulletType BulletType;
	
protected:
	virtual void HandlePickUp(AFDPlayerCharacter* Player) override;
	
	void BulletEnhanced(AFDPlayerCharacter* Player);
	void BulletGeneral(AFDPlayerCharacter* Player);
	void BulletImpact(AFDPlayerCharacter* Player);
	void BulletHighPower(AFDPlayerCharacter* Player);
};
