// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HudWeaponInfoWidget.generated.h"

struct FFDEquipmentEntry;
class UInventorySlotTextures;
struct FGameplayTag;
class UHorizontalBox;
class UTextBlock;
class UImage;

DECLARE_DELEGATE_OneParam(FLeftAmmoChangeSignature, float);
/**
 * 
 */
UCLASS()
class PROJECTFD_API UHudWeaponInfoWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void SetSlotInfo(const FFDEquipmentEntry& InEntry, float LeftAmmo);
	void SetEquipped();
	void SetUnEquipped();

	void InitializeSelectGageInfo(int SlotNum);

	FLeftAmmoChangeSignature LeftAmmoChangeDelegate;

	UFUNCTION()
	void SetLeftAmmoText(float LeftAmmo);
	
protected:

	void SetWeaponInfoContainer(bool bIsActive);
	void SetStatusInfoText(bool bIsActive);
	void SetChargedAmmoText(float ChargedAmmo);
	void SetWeaponTypeImage(FGameplayTag ItemTag);
	void SetAmmoTypeImage(FGameplayTag AmmoType);
	void SetActiveWeaponSpacer(bool bIsActive);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="HudWeaponInfo", meta = (BindWidget, AllowPrivateAccess=true))
	UHorizontalBox* HudWeaponInfoContainer;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="HudWeaponInfo", meta = (BindWidget, AllowPrivateAccess=true))
	UTextBlock* StatusInfoText;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="HudWeaponInfo", meta = (BindWidget, AllowPrivateAccess=true))
	UTextBlock* ChargedAmmoText;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="HudWeaponInfo", meta = (BindWidget, AllowPrivateAccess=true))
	UTextBlock* LeftAmmoText;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="HudWeaponInfo", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* WeaponTypeImage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="HudWeaponInfo", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* AmmoTypeImage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="HudWeaponInfo", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* WeaponSelectGageImage1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="HudWeaponInfo", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* WeaponSelectGageImage2;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="HudWeaponInfo", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* WeaponSelectGageImage3;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="HudWeaponInfo", meta = (BindWidget, AllowPrivateAccess=true))
	class USpacer* ActiveWeaponSpacer;

	// Custom Values
	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Asset", meta=(AllowPrivateAccess=true))
	UInventorySlotTextures* HudWeaponInfoTextures;
	
	
};
