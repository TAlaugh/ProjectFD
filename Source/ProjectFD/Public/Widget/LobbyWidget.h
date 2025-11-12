// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

class UTextBlock;
class UImage;
class UButton;
/**
 * 
 */
UCLASS()
class PROJECTFD_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;

	void SetButtonInfo(bool bIsReady, int ReadyNum, int MaxNum);


protected:

	UFUNCTION()
	void OnBattleStartButtonClicked();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Lobby|Ready", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* ButtonStartImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Lobby|Ready", meta = (BindWidget, AllowPrivateAccess=true))
	UImage* ButtonReadyImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Lobby|Ready", meta = (BindWidget, AllowPrivateAccess=true))
	UButton* BattleStartButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Lobby|Ready", meta = (BindWidget, AllowPrivateAccess=true))
	UTextBlock* ReadyText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Lobby|Ready", meta = (BindWidget, AllowPrivateAccess=true))
	UTextBlock* ReadyCountDownText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Lobby|Ready", meta = (BindWidget, AllowPrivateAccess=true))
	UTextBlock* ReadyNumText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Lobby|Ready", meta = (BindWidget, AllowPrivateAccess=true))
	UTextBlock* TotalNumText;

	float CountDownNum = 5.f;
	FNumberFormattingOptions CountNumOption;
	FTimerHandle CountDownHandle;
};
