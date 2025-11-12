// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/WidgetController/BaseWidgetController.h"
#include "PlayerHudWidgetController.generated.h"

struct FGameplayTag;
struct FFDEquipmentEntry;
class AFDPlayerCharacter;

DECLARE_MULTICAST_DELEGATE_OneParam(FWidgetHealthChangeSignature, float)
DECLARE_MULTICAST_DELEGATE_OneParam(FWidgetMaxHealthChangeSignature, float)
DECLARE_MULTICAST_DELEGATE_OneParam(FWidgetShieldChangeSignature, float)
DECLARE_MULTICAST_DELEGATE_OneParam(FWidgetMaxShieldChangeSignature, float)
DECLARE_MULTICAST_DELEGATE_OneParam(FWidgetAmmoChangeSignature, float)
DECLARE_MULTICAST_DELEGATE_OneParam(FWidgetGeneralAmmoChangeSignature, float)
DECLARE_MULTICAST_DELEGATE_OneParam(FWidgetSpecialAmmoChangeSignature, float)
DECLARE_MULTICAST_DELEGATE_OneParam(FWidgetImpactAmmoChangeSignature, float)
DECLARE_MULTICAST_DELEGATE_OneParam(FWidgetHighPowerAmmoChangeSignature, float)
DECLARE_MULTICAST_DELEGATE_OneParam(FWidgetManaChangeSignature, float)
DECLARE_MULTICAST_DELEGATE_OneParam(FWidgetMaxManaChangeSignature, float)
DECLARE_MULTICAST_DELEGATE_OneParam(FWidgetSpecialResourceChangeSignature, float)
DECLARE_MULTICAST_DELEGATE_OneParam(FWidgetMaxSpecialResourceChangeSignature, float)

DECLARE_MULTICAST_DELEGATE_OneParam(FHudEquipmentEntrySignature, const FFDEquipmentEntry&)
DECLARE_MULTICAST_DELEGATE(FHudEquipmentEntryResetSignature)

/**
 * 
 */
UCLASS()
class PROJECTFD_API UPlayerHudWidgetController : public UBaseWidgetController
{
	GENERATED_BODY()
	
public:

	AActor* GetOwningActor();
	void SetOwningActor(AActor* InOwner);

	void BindCallbacksToDependencies();
	void BroadcastInitialValues();
	
	FWidgetHealthChangeSignature WidgetHealthChangeDelegate;
	FWidgetMaxHealthChangeSignature WidgetMaxHealthChangeDelegate;
	FWidgetShieldChangeSignature WidgetShieldChangeDelegate;
	FWidgetMaxShieldChangeSignature WidgetMaxShieldChangeDelegate;
	FWidgetAmmoChangeSignature WidgetAmmoChangeDelegate;
	FWidgetGeneralAmmoChangeSignature WidgetGeneralAmmoChangeDelegate;
	FWidgetSpecialAmmoChangeSignature WidgetSpecialAmmoChangeDelegate;
	FWidgetImpactAmmoChangeSignature WidgetImpactAmmoChangeDelegate;
	FWidgetHighPowerAmmoChangeSignature WidgetHighPowerAmmoChangeDelegate;
	FWidgetManaChangeSignature WidgetManaChangeDelegate;
	FWidgetMaxManaChangeSignature WidgetMaxManaChangeDelegate;
	FWidgetSpecialResourceChangeSignature WidgetSpecialResourceChangeDelegate;
	FWidgetMaxSpecialResourceChangeSignature WidgetMaxSpecialResourceChangeDelegate;

	FHudEquipmentEntrySignature HudEquipmentEntryDelegate;
	FHudEquipmentEntryResetSignature HudEquipmentEntryResetDelegate;
	
private:

	UPROPERTY()
	TObjectPtr<AActor> OwningActor;

	
};
