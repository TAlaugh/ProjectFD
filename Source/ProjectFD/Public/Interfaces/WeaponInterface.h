// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "WeaponInterface.generated.h"

class AEquipmentActor;

UINTERFACE(MinimalAPI)
class UWeaponInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTFD_API IWeaponInterface
{
	GENERATED_BODY()


public:

	virtual AEquipmentActor* GetWeaponActor() = 0;

	virtual void SetWeaponActor(AEquipmentActor* WeaponActor) = 0;
	
};
