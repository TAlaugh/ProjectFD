// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "WeaponHoveringWidget.generated.h"

struct FFDInventoryEntry;
struct FEquipmentStatEffectGroup;
struct FGameplayTag;
struct FFDEquipmentEntry;
class UVerticalBox;
class UTextBlock;
class UWeaponOptionSlotWidget;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UWeaponHoveringWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;

	void SetSlotInfo(FFDInventoryEntry& InEntry, FGameplayTag RarityTag, float FireTime);
	void SetSlotInfo(FFDEquipmentEntry& InEntry);


protected:

	void SetTierInfo(FGameplayTag TierTag);
	void SetLevelText(float InLevel);
	void SetWeaponNameText(FText NameText);
	void SetDPSText(float InDPS);
	void SetAmmoTypeInfo(FGameplayTag AmmoType);
	void SetWeaponDamageText(float InDamage);
	void SetAmmoText(float InAmmo);
	void SetRPMText(float InRPM);


private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess=true))
	TObjectPtr<UImage> TierImage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess=true))
	TObjectPtr<UTextBlock> WeaponLevelText;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess=true))
	TObjectPtr<UTextBlock> WeaponNameText;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess=true))
	TObjectPtr<UTextBlock> WeaponTierText;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess=true))
	TObjectPtr<UTextBlock> DPSText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess=true))
	TObjectPtr<UImage> AmmoTypeImage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess=true))
	TObjectPtr<UTextBlock> AmmoTypeText;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess=true))
	TObjectPtr<UTextBlock> WeaponDamageText;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess=true))
	TObjectPtr<UTextBlock> AmmoText;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess=true))
	TObjectPtr<UTextBlock> RoundPerMinText;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess=true))
	TObjectPtr<UVerticalBox> WeaponOptionContainer;

	
	UPROPERTY(EditDefaultsOnly, Category="Custom Values|SlotWidget", meta=(AllowPrivateAccess=true))
	TSubclassOf<UWeaponOptionSlotWidget> WeaponOptionSlotClass;
	
	
};
