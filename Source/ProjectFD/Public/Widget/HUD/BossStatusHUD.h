// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossStatusHUD.generated.h"

class UTextBlock;
class UImage;
class AFDBossCharacter;
class UProgressBar;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UBossStatusHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void SetBossRef(AFDBossCharacter* InBoss);
	
	UFUNCTION(BlueprintCallable)
	void SetBossInfo(float InCurrentHp, float InMaxHp, float InCurrentShield, float InMaxShield);
	
	UFUNCTION(BlueprintCallable)
	void FadeOutImage();

	UFUNCTION(BlueprintCallable)
	void FadeInImage();

	UFUNCTION(BlueprintCallable)
	void SuccessAnimation();
	
	UFUNCTION(BlueprintCallable)
	void FailedAnimation();

	UFUNCTION(BlueprintCallable)
	void SetDeathCountText(int32 DeathCount);
	
protected:
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeAnim;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* SuccessAnim;
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FailedAnim;
	
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HP_Bar;
	
	UPROPERTY(meta = (BindWidget))
	UProgressBar* Shield_Bar;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DeathCountText;

	UPROPERTY()
	AFDBossCharacter* BossRef;
};
