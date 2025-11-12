// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/HUD/BossStatusHUD.h"

#include "Character/Monster/Boss/FDBossCharacter.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Gameframework/FDGameMode.h"
#include "Gameframework/FDGameState.h"
#include "GAS/Monster/EnemyAttributeSet.h"

void UBossStatusHUD::NativeConstruct()
{
	Super::NativeConstruct();

	if (BossRef)
	{
		// Delegate 바인딩
		BossRef->OnBossStatChanged.AddDynamic(this, &UBossStatusHUD::SetBossInfo);

		// 초기값 세팅
		SetBossInfo(
			BossRef->GetEnemyAttributeSet()->GetHealth(),
			BossRef->GetEnemyAttributeSet()->GetMaxHealth(),
			BossRef->GetEnemyAttributeSet()->GetShield(),
			BossRef->GetEnemyAttributeSet()->GetMaxShield()
		);

	}

	SetDeathCountText(GetWorld()->GetGameState<AFDGameState>()->SharedRespawns - 1);
}

void UBossStatusHUD::SetBossRef(AFDBossCharacter* InBoss)
{
	if (!InBoss) return;

	BossRef = InBoss;

	if (IsConstructed()) 
	{
		BossRef->OnBossStatChanged.AddDynamic(this, &UBossStatusHUD::SetBossInfo);

		SetBossInfo(
			BossRef->GetEnemyAttributeSet()->GetHealth(),
			BossRef->GetEnemyAttributeSet()->GetMaxHealth(),
			BossRef->GetEnemyAttributeSet()->GetShield(),
			BossRef->GetEnemyAttributeSet()->GetMaxShield()
		);
	}
}

void UBossStatusHUD::SetBossInfo(float InCurrentHp, float InMaxHp, float InCurrentShield, float InMaxShield)
{
	if (InCurrentHp > 0)
	{
		HP_Bar->SetVisibility(ESlateVisibility::Visible);
		Shield_Bar->SetVisibility(ESlateVisibility::Visible);
		
		HP_Bar->SetPercent(InCurrentHp / InMaxHp);
		Shield_Bar->SetPercent(InCurrentShield / InMaxShield);
	}
	if (InCurrentHp <= 0)
	{
		HP_Bar->SetVisibility(ESlateVisibility::Hidden);
		Shield_Bar->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UBossStatusHUD::FadeOutImage()
{
	if (FadeAnim)
	{
		PlayAnimationReverse(FadeAnim);
	}
}

void UBossStatusHUD::FadeInImage()
{
	if (FadeAnim)
	{
		PlayAnimation(FadeAnim);
	}
}

void UBossStatusHUD::SuccessAnimation()
{
	if (SuccessAnim)
	{
		PlayAnimation(SuccessAnim);
	}
}

void UBossStatusHUD::FailedAnimation()
{
	if (FailedAnim)
	{
		PlayAnimation(FailedAnim);
	}
}

void UBossStatusHUD::SetDeathCountText(int32 DeathCount)
{
	if (DeathCount == 0)
	{
		DeathCountText->SetColorAndOpacity(FSlateColor(FLinearColor(1.f, 0.f, 0.f, 1.f)));
	}
	DeathCountText->SetText(FText::AsNumber(DeathCount));
}
