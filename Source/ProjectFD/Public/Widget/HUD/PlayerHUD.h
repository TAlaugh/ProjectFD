// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/BaseWidget.h"
#include "Delegates/Delegate.h"
#include "PlayerHUD.generated.h"

struct FGameplayTag;
class UHudSkillInfoSlotWidget;
struct FFDEquipmentEntry;
class UHudWeaponInfoWidget;
class UPlayerHudWidgetController;
class UHorizontalBox;
class URetainerBox;
class UProgressBar;
class UTextBlock;
class UVerticalBox;

DECLARE_MULTICAST_DELEGATE_OneParam(FSkillCoolDownStartSignature, FGameplayTag)

UENUM()
enum EAmmoType
{
	General,
	Special,
	Impact,
	HighPower
};

/**
 * 
 */
UCLASS()
class PROJECTFD_API UPlayerHUD : public UBaseWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void OnWidgetControllerSet() override;



protected:

	UPROPERTY()
	UPlayerHudWidgetController* CachedPlayerHudController;

#pragma region HudWeaponInfo

public:

	void SetWeaponInfoToDefault(FFDEquipmentEntry UnEquippedEntry);

protected:

	void SetWeaponInfosToDefaultTemporary();

	UHudWeaponInfoWidget* GetHudWeaponSlotBySlotNum(int SlotNum);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="HudWeaponInfo", meta = (BindWidget, AllowPrivateAccess=true))
	UVerticalBox* HudWeaponInfoContainer;

	// UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="HudWeaponInfo", meta = (BindWidget, AllowPrivateAccess=true))
	// UHudWeaponInfoWidget* HudWeaponSlot1;
	// UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="HudWeaponInfo", meta = (BindWidget, AllowPrivateAccess=true))
	// UHudWeaponInfoWidget* HudWeaponSlot2;
	// UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="HudWeaponInfo", meta = (BindWidget, AllowPrivateAccess=true))
	// UHudWeaponInfoWidget* HudWeaponSlot3;
	UPROPERTY()
	UHudWeaponInfoWidget* HudWeaponSlot1;
	UPROPERTY()
	UHudWeaponInfoWidget* HudWeaponSlot2;
	UPROPERTY()
	UHudWeaponInfoWidget* HudWeaponSlot3;

	void SetHudWeaponInfo(const FFDEquipmentEntry& InEntry);

	UFUNCTION()
	void SetHudWeaponSlot1(float LeftAmmo);
	UFUNCTION()
	void SetHudWeaponSlot2(float LeftAmmo);
	UFUNCTION()
	void SetHudWeaponSlot3(float LeftAmmo);

	void BindHudWeaponSlotDelegate(EAmmoType AmmoType, int SlotNum);

	FDelegateHandle EquipmentEntryDelegateHandle = FDelegateHandle();
	FDelegateHandle HudWeaponSlotHandle1 = FDelegateHandle();
	FDelegateHandle HudWeaponSlotHandle2 = FDelegateHandle();
	FDelegateHandle HudWeaponSlotHandle3 = FDelegateHandle();

	// Custom Values
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="CustomValue|Class", meta = (AllowPrivateAccess=true))
	TSubclassOf<UHudWeaponInfoWidget> HudWeaponInfoWidgetClass;

#pragma endregion

#pragma region Ammo&Status

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AmmoStatus", meta = (BindWidget, AllowPrivateAccess=true))
	UTextBlock* CurrentAmmoText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AmmoStatus", meta = (BindWidget, AllowPrivateAccess=true))
	UTextBlock* LeftAmmoText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AmmoStatus", meta = (BindWidget, AllowPrivateAccess=true))
	UProgressBar* ShieldProgressBar;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AmmoStatus", meta = (BindWidget, AllowPrivateAccess=true))
	URetainerBox* ShieldRetainerBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AmmoStatus", meta = (BindWidget, AllowPrivateAccess=true))
	UHorizontalBox* ShieldImageContainer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AmmoStatus", meta = (BindWidget, AllowPrivateAccess=true))
	UProgressBar* HealthProgressBar;


private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="CustomValue|Asset", meta = (AllowPrivateAccess=true))
	UTexture2D* ShieldImage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="CustomValue|Asset", meta = (AllowPrivateAccess=true))
	UMaterialInterface* ShieldRetainerMaterial;

	UPROPERTY()
	UMaterialInstanceDynamic* ShieldRetainerMID = nullptr;

	FDelegateHandle MainLeftAmmoHandle = FDelegateHandle();

	void BindCallbacksToDependencies();

	UFUNCTION()
	void SetHealth(float NewValue);
	UFUNCTION()
	void SetMaxHealth(float NewValue);
	UFUNCTION()
	void SetShield(float NewValue);
	UFUNCTION()
	void SetMaxShield(float NewValue);
	UFUNCTION()
	void SetAmmo(float NewValue);
	UFUNCTION()
	void SetLeftAmmo(float NewValue);

	UFUNCTION()
	void SetHealthBar();
	UFUNCTION()
	void SetShieldBar(bool bIsMaxChanged);


	UPROPERTY()
	float Health = 0;
	UPROPERTY()
	float MaxHealth = 0;
	UPROPERTY()
	float Shield = 200;
	UPROPERTY()
	float MaxShield = 0;
	UPROPERTY()
	int Ammo = 0;
	UPROPERTY()
	int GeneralAmmo = 0;
	

#pragma endregion

#pragma region BottomInfos

public:

	UFUNCTION()
	void SetSkillCoolDownImage(FGameplayTag CooldownTag);

	FSkillCoolDownStartSignature SkillCoolDownStartDelegate;
	

protected:

	void SetManaLackImage();
	void SetSpecialResourceLackImage();

	void SetManaValue(float InCurrentMana);
	void SetMaxManaValue(float InMaxMana);
	void SetManaProgressBar();

	void SetSpecialResourceValue(float InCurrentSpecialResource);
	void SetMaxSpecialResourceValue(float InMaxSpecialResource);
	void SetSpecialResourceProgressBar();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BottomInfos", meta = (BindWidget, AllowPrivateAccess=true))
	UHorizontalBox* SkillInfoContainer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BottomInfos", meta = (BindWidget, AllowPrivateAccess=true))
	UProgressBar* ManaProgressBar;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BottomInfos", meta = (BindWidget, AllowPrivateAccess=true))
	UProgressBar* SpecialResourceProgressBar;

	UPROPERTY()
	UHudSkillInfoSlotWidget* FirstSkillInfo;
	UPROPERTY()
	UHudSkillInfoSlotWidget* SecondSkillInfo;
	UPROPERTY()
	UHudSkillInfoSlotWidget* ThirdSkillInfo;
	UPROPERTY()
	UHudSkillInfoSlotWidget* FourthSkillInfo;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="CustomValue|Asset", meta = (AllowPrivateAccess=true))
	TObjectPtr<UTexture2D> FirstSkillTexture;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="CustomValue|Asset", meta = (AllowPrivateAccess=true))
	TObjectPtr<UTexture2D> SecondSkillTexture;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="CustomValue|Asset", meta = (AllowPrivateAccess=true))
	TObjectPtr<UTexture2D> ThirdSkillTexture;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="CustomValue|Asset", meta = (AllowPrivateAccess=true))
	TObjectPtr<UTexture2D> FourthSkillTexture;


private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="CustomValue|Class", meta = (AllowPrivateAccess=true))
	TSubclassOf<UHudSkillInfoSlotWidget> HudSkillInfoSlotWidgetClass;

	float CurrentMana = 0.f;
	float MaxMana = 0.f;
	float CurrentSpecialResource = 0.f;
	float MaxSpecialResource = 0.f;
	


#pragma endregion
	
	
};
