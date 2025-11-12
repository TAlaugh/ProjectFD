// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HudSkillInfoSlotWidget.generated.h"

struct FGameplayTag;
class UTextBlock;
class UImage;

/**
 * 
 */
UCLASS()
class PROJECTFD_API UHudSkillInfoSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void SetSlotInfo(FText KeyText, UTexture2D* IconTexture);
	void SetManaLackImage(bool bIsReady);
	void SetSkillCoolTimeImage(float OutTimeRemaining, float OutTotalTime);
	
protected:

	void SetSkillKeyText(FText InText);
	void SetSkillIcon(UTexture2D* InTexture);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="SkillSlot", meta = (BindWidget, AllowPrivateAccess=true))
	UTextBlock* SkillKeyText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="SkillSlot", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* SkillIconImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="SkillSlot", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* SkillManaLackImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="SkillSlot", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* SkillCoolTimeImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="CustomValue|Asset", meta = (AllowPrivateAccess=true))
	UMaterialInterface* SkillCoolDownMaterial;

	UPROPERTY()
	UMaterialInstanceDynamic* DynMat = nullptr;

	FTimerHandle SkillTimerHandle;
	float RemainingTime = 0.f;
	float TotalTime = 0.f;
	
};
