// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/FDCharacter.h"
#include "FDPreviewCharacter.generated.h"

class AEquipmentActor;
class AFDPlayerCharacter;
/**
 * 
 */
UCLASS()
class PROJECTFD_API AFDPreviewCharacter : public AFDCharacter
{
	GENERATED_BODY()
	
public:
	
	AFDPreviewCharacter();

	void ChangeAnimLayer(TSubclassOf<UAnimInstance> AnimInstance);
	void EquipPreviewWeapon(AEquipmentActor* WeaponActor);

	UTextureRenderTarget2D* GetRenderTarget2D();

	UPROPERTY(EditDefaultsOnly)
	USceneCaptureComponent2D* SceneCaptureComponent;

	UPROPERTY()
	UTextureRenderTarget2D* RenderTarget2D;

	UPROPERTY(BlueprintReadOnly)
	AEquipmentActor* PreviewWeapon;
	
};
