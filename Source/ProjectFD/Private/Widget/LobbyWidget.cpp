// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/LobbyWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Controller/FDPlayerController.h"


void ULobbyWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BattleStartButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnBattleStartButtonClicked);
}

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ButtonStartImage->SetColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 1.f));
	ButtonReadyImage->SetColorAndOpacity(FLinearColor(0.f, 1.f, 0.4f, 1.f));
	ReadyCountDownText->SetOpacity(0.f);
}

void ULobbyWidget::SetButtonInfo(bool bIsReady, int ReadyNum, int MaxNum)
{
	if (bIsReady)
	{
		if (ReadyNum == MaxNum)
		{
			ButtonStartImage->SetOpacity(0.f);
			ButtonReadyImage->SetColorAndOpacity(FLinearColor(0.f, 1.f, 0.4f, 1.f));
			ReadyText->SetText(FText::FromString(TEXT("입장 중입니다")));
			ReadyText->SetColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 1.f));

			ReadyCountDownText->SetOpacity(1.f);

			CountNumOption.MinimumFractionalDigits = 0;
			CountNumOption.MaximumFractionalDigits = 0;
			
			CountDownNum = 5.f;
			GetWorld()->GetTimerManager().SetTimer(CountDownHandle, [this]()
			{
				CountDownNum -= 0.1f;
				if (CountDownNum <= 0.f)
				{
					GetWorld()->GetTimerManager().ClearTimer(CountDownHandle);
					CountDownHandle.Invalidate();
					return;
				}
				ReadyCountDownText->SetText(FText::AsNumber(CountDownNum, &CountNumOption));
			},
			0.1f,
			true);
		}
		else
		{
			ButtonStartImage->SetOpacity(0.f);
			ButtonReadyImage->SetColorAndOpacity(FLinearColor(0.1f, 0.4f, 1.f, 1.f));
			ReadyText->SetText(FText::FromString(TEXT("요격 입장 대기")));
			ReadyText->SetColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 1.f));

			ReadyCountDownText->SetOpacity(0.f);
			GetWorld()->GetTimerManager().ClearTimer(CountDownHandle);
			CountDownHandle.Invalidate();
		}
	}
	else
	{
		ButtonStartImage->SetOpacity(1.f);
		ButtonReadyImage->SetOpacity(0.f);
		ReadyText->SetText(FText::FromString(TEXT("요격 준비")));
		ReadyText->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));

		ReadyCountDownText->SetOpacity(0.f);
		GetWorld()->GetTimerManager().ClearTimer(CountDownHandle);
		CountDownHandle.Invalidate();
	}
	
	ReadyNumText->SetText(FText::AsNumber(ReadyNum));
	TotalNumText->SetText(FText::AsNumber(MaxNum));
}

void ULobbyWidget::OnBattleStartButtonClicked()
{
	Cast<AFDPlayerController>(GetOwningPlayer())->GetReadyForBattle();
}
