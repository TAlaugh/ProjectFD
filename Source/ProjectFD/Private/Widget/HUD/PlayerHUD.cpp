// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/HUD/PlayerHUD.h"

#include "Character/Player/FDPlayerCharacter.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/RetainerBox.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Equipment/EquipmentManagerComponent.h"
#include "Gameframework/FDPlayerState.h"
#include "GAS/FDGameplayStatic.h"
#include "GAS/Player/PlayerAttributeSet.h"
#include "Widget/HUD/HudWeaponInfoWidget.h"
#include "Widget/HUD/HudSkillInfoSlotWidget.h"
#include "Widget/WidgetController/PlayerHudWidgetController.h"


void UPlayerHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();

#pragma region HudWeaponSlots
	HudWeaponSlot1 = CreateWidget<UHudWeaponInfoWidget>(this, HudWeaponInfoWidgetClass);
	HudWeaponSlot2 = CreateWidget<UHudWeaponInfoWidget>(this, HudWeaponInfoWidgetClass);
	HudWeaponSlot3 = CreateWidget<UHudWeaponInfoWidget>(this, HudWeaponInfoWidgetClass);

	HudWeaponSlot1->InitializeSelectGageInfo(0);
	HudWeaponSlot2->InitializeSelectGageInfo(1);
	HudWeaponSlot3->InitializeSelectGageInfo(2);

	HudWeaponInfoContainer->AddChild(HudWeaponSlot1);

	// Add Spacer
	USpacer* NewSpacer = NewObject<USpacer>(this);
	NewSpacer->SetSize(FVector2D(1.f, 10.f));
	HudWeaponInfoContainer->AddChild(NewSpacer);
	
	HudWeaponInfoContainer->AddChild(HudWeaponSlot2);

	// Add Spacer
	NewSpacer = NewObject<USpacer>(this);
	NewSpacer->SetSize(FVector2D(1.f, 10.f));
	HudWeaponInfoContainer->AddChild(NewSpacer);
	
	HudWeaponInfoContainer->AddChild(HudWeaponSlot3);
#pragma endregion

#pragma region BottomSkillInfos

	FirstSkillInfo = CreateWidget<UHudSkillInfoSlotWidget>(this, HudSkillInfoSlotWidgetClass);
	SecondSkillInfo = CreateWidget<UHudSkillInfoSlotWidget>(this, HudSkillInfoSlotWidgetClass);
	ThirdSkillInfo = CreateWidget<UHudSkillInfoSlotWidget>(this, HudSkillInfoSlotWidgetClass);
	FourthSkillInfo = CreateWidget<UHudSkillInfoSlotWidget>(this, HudSkillInfoSlotWidgetClass);

	FirstSkillInfo->SetSlotInfo(FText::FromString(TEXT("Q")), FirstSkillTexture);
	SecondSkillInfo->SetSlotInfo(FText::FromString(TEXT("C")), SecondSkillTexture);
	ThirdSkillInfo->SetSlotInfo(FText::FromString(TEXT("V")), ThirdSkillTexture);
	FourthSkillInfo->SetSlotInfo(FText::FromString(TEXT("Z")), FourthSkillTexture);

	SkillInfoContainer->AddChild(FirstSkillInfo);
	SkillInfoContainer->AddChild(SecondSkillInfo);
	SkillInfoContainer->AddChild(ThirdSkillInfo);
	SkillInfoContainer->AddChild(FourthSkillInfo);

	SkillCoolDownStartDelegate.AddUObject(this, &UPlayerHUD::SetSkillCoolDownImage);

#pragma endregion
}

void UPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();

	OnWidgetControllerSet();

	EquipmentEntryDelegateHandle = CachedPlayerHudController->HudEquipmentEntryDelegate.AddLambda(
		[this](const FFDEquipmentEntry& Entry)
		{
			SetHudWeaponInfo(Entry);
		});

	CachedPlayerHudController->HudEquipmentEntryResetDelegate.AddLambda(
		[this]()
		{
			SetWeaponInfosToDefaultTemporary();
		});

	if (ShieldRetainerBox && ShieldRetainerMaterial)
	{
		ShieldRetainerMID = UMaterialInstanceDynamic::Create(ShieldRetainerMaterial, this);

		ShieldRetainerBox->SetEffectMaterial(ShieldRetainerMID);
	}
	if (AFDPlayerState* PlayerState = GetOwningPlayer()->GetPlayerState<AFDPlayerState>())
	{
		if (UAbilitySystemComponent* ASC = PlayerState->GetAbilitySystemComponent())
		{
			SetMaxHealth(ASC->GetNumericAttribute(UPlayerAttributeSet::GetMaxHealthAttribute()));
			SetMaxShield(ASC->GetNumericAttribute(UPlayerAttributeSet::GetMaxShieldAttribute()));
			SetHealth(ASC->GetNumericAttribute(UPlayerAttributeSet::GetHealthAttribute()));
			SetShield(ASC->GetNumericAttribute(UPlayerAttributeSet::GetShieldAttribute()));
		}
	}

	// FTimerHandle TempTimerHandle;
	// GetWorld()->GetTimerManager().SetTimer(TempTimerHandle,[this] ()
	// 	{
	// 		FirstSkillInfo->SetSkillCoolTimeImage(5.f, 5.f);
	// 	},
	// 	3.f,
	// 	false);
	// FTimerHandle TempTimerHandle2;
	// GetWorld()->GetTimerManager().SetTimer(TempTimerHandle2,[this] ()
	// 	{
	// 		ThirdSkillInfo->SetSkillCoolTimeImage(6.f, 7.f);
	// 	},
	// 	6.f,
	// 	false);
	
}

void UPlayerHUD::NativeDestruct()
{
	Super::NativeDestruct();

	// CachedPlayerHudController->HudEquipmentEntryDelegate.Remove(EquipmentEntryDelegateHandle);
	// EquipmentEntryDelegateHandle.Reset();
}

void UPlayerHUD::OnWidgetControllerSet()
{
	Super::OnWidgetControllerSet();

	checkf(WidgetController, TEXT("InventoryMainWidget : WidgetController not set"));
	
	CachedPlayerHudController = Cast<UPlayerHudWidgetController>(WidgetController);

	BindCallbacksToDependencies();
}

void UPlayerHUD::SetWeaponInfoToDefault(FFDEquipmentEntry UnEquippedEntry)
{
	if (UnEquippedEntry.bIsActiveWeapon)
	{
		MainLeftAmmoHandle.Reset();
		SetAmmo(0);
		SetLeftAmmo(0);
	}

	GetHudWeaponSlotBySlotNum(UnEquippedEntry.SlotNum)->SetUnEquipped();
}

void UPlayerHUD::SetWeaponInfosToDefaultTemporary()
{
	MainLeftAmmoHandle.Reset();
	SetAmmo(0);
	SetLeftAmmo(0);
	
	HudWeaponSlot1->SetUnEquipped();
	HudWeaponSlot2->SetUnEquipped();
	HudWeaponSlot3->SetUnEquipped();
}

UHudWeaponInfoWidget* UPlayerHUD::GetHudWeaponSlotBySlotNum(int SlotNum)
{
	if (SlotNum == 0)
	{
		return HudWeaponSlot1;
	}
	else if (SlotNum == 1)
	{
		return HudWeaponSlot2;
	}
	else if (SlotNum == 2)
	{
		return HudWeaponSlot3;
	}
	else
	{
		return nullptr;
	}
}

void UPlayerHUD::SetHudWeaponInfo(const FFDEquipmentEntry& InEntry)
{
	float CurrentLeftAmmo = 0;
	if (AFDPlayerState* PlayerState = GetOwningPlayer()->GetPlayerState<AFDPlayerState>())
	{
		if (UAbilitySystemComponent* ASC = PlayerState->GetAbilitySystemComponent())
		{
			if (InEntry.AmmoType.MatchesTag(FGameplayTag::RequestGameplayTag("Equipment.AmmoType.General")))
			{
				if (UHudWeaponInfoWidget* CachedWeaponWidget = GetHudWeaponSlotBySlotNum(InEntry.SlotNum))
				{
					CurrentLeftAmmo = ASC->GetNumericAttribute(UPlayerAttributeSet::GetGeneralAmmoAttribute());
					if (InEntry.bIsActiveWeapon)
					{
						MainLeftAmmoHandle.Reset();
						MainLeftAmmoHandle = CachedPlayerHudController->WidgetGeneralAmmoChangeDelegate.AddUObject(this, &UPlayerHUD::SetLeftAmmo);
						SetLeftAmmo(CurrentLeftAmmo);
					}
					else
					{
						BindHudWeaponSlotDelegate(General, InEntry.SlotNum);
					}
					CachedWeaponWidget->SetSlotInfo(InEntry, CurrentLeftAmmo);
				}
			}
			else if (InEntry.AmmoType.MatchesTag(FGameplayTag::RequestGameplayTag("Equipment.AmmoType.Special")))
			{
				if (UHudWeaponInfoWidget* CachedWeaponWidget = GetHudWeaponSlotBySlotNum(InEntry.SlotNum))
				{
					CurrentLeftAmmo = ASC->GetNumericAttribute(UPlayerAttributeSet::GetSpecialAmmoAttribute());
					if (InEntry.bIsActiveWeapon)
					{
						MainLeftAmmoHandle.Reset();
						MainLeftAmmoHandle = CachedPlayerHudController->WidgetSpecialAmmoChangeDelegate.AddUObject(this, &UPlayerHUD::SetLeftAmmo);
						SetLeftAmmo(CurrentLeftAmmo);
					}
					else
					{
						BindHudWeaponSlotDelegate(Special, InEntry.SlotNum);
					}
					CachedWeaponWidget->SetSlotInfo(InEntry, CurrentLeftAmmo);
				}
			}
			else if (InEntry.AmmoType.MatchesTag(FGameplayTag::RequestGameplayTag("Equipment.AmmoType.Impact")))
			{
				if (UHudWeaponInfoWidget* CachedWeaponWidget = GetHudWeaponSlotBySlotNum(InEntry.SlotNum))
				{
					CurrentLeftAmmo = ASC->GetNumericAttribute(UPlayerAttributeSet::GetImpactAmmoAttribute());
					if (InEntry.bIsActiveWeapon)
					{
						MainLeftAmmoHandle.Reset();
						MainLeftAmmoHandle = CachedPlayerHudController->WidgetImpactAmmoChangeDelegate.AddUObject(this, &UPlayerHUD::SetLeftAmmo);
						SetLeftAmmo(CurrentLeftAmmo);
					}
					else
					{
						BindHudWeaponSlotDelegate(Impact, InEntry.SlotNum);
					}
					CachedWeaponWidget->SetSlotInfo(InEntry, CurrentLeftAmmo);
				}
			}
			else if (InEntry.AmmoType.MatchesTag(FGameplayTag::RequestGameplayTag("Equipment.AmmoType.HighPower")))
			{
				if (UHudWeaponInfoWidget* CachedWeaponWidget = GetHudWeaponSlotBySlotNum(InEntry.SlotNum))
				{
					CurrentLeftAmmo = ASC->GetNumericAttribute(UPlayerAttributeSet::GetHighPowerAmmoAttribute());
					if (InEntry.bIsActiveWeapon)
					{
						MainLeftAmmoHandle.Reset();
						MainLeftAmmoHandle = CachedPlayerHudController->WidgetHighPowerAmmoChangeDelegate.AddUObject(this, &UPlayerHUD::SetLeftAmmo);
						SetLeftAmmo(CurrentLeftAmmo);
					}
					else
					{
						BindHudWeaponSlotDelegate(HighPower, InEntry.SlotNum);
					}
					CachedWeaponWidget->SetSlotInfo(InEntry, CurrentLeftAmmo);
				}
			}
		}
	}
}

void UPlayerHUD::SetHudWeaponSlot1(float LeftAmmo)
{
	HudWeaponSlot1->LeftAmmoChangeDelegate.ExecuteIfBound(LeftAmmo);
}

void UPlayerHUD::SetHudWeaponSlot2(float LeftAmmo)
{
	HudWeaponSlot2->LeftAmmoChangeDelegate.ExecuteIfBound(LeftAmmo);
}

void UPlayerHUD::SetHudWeaponSlot3(float LeftAmmo)
{
	HudWeaponSlot3->LeftAmmoChangeDelegate.ExecuteIfBound(LeftAmmo);
}

void UPlayerHUD::BindHudWeaponSlotDelegate(EAmmoType AmmoType, int SlotNum)
{
	switch (AmmoType)
	{
	case General:
		if (SlotNum == 0)
		{
			HudWeaponSlotHandle1.Reset();
			HudWeaponSlotHandle1 = CachedPlayerHudController->WidgetGeneralAmmoChangeDelegate.AddUObject(this, &UPlayerHUD::SetHudWeaponSlot1);
		}
		else if (SlotNum == 1)
		{
			HudWeaponSlotHandle2.Reset();
			HudWeaponSlotHandle2 = CachedPlayerHudController->WidgetGeneralAmmoChangeDelegate.AddUObject(this, &UPlayerHUD::SetHudWeaponSlot2);
		}
		else if (SlotNum == 2)
		{
			HudWeaponSlotHandle3.Reset();
			HudWeaponSlotHandle3 = CachedPlayerHudController->WidgetGeneralAmmoChangeDelegate.AddUObject(this, &UPlayerHUD::SetHudWeaponSlot3);
		}
		break;
	case Special:
		if (SlotNum == 0)
		{
			HudWeaponSlotHandle1.Reset();
			HudWeaponSlotHandle1 = CachedPlayerHudController->WidgetSpecialAmmoChangeDelegate.AddUObject(this, &UPlayerHUD::SetHudWeaponSlot1);
		}
		else if (SlotNum == 1)
		{
			HudWeaponSlotHandle2.Reset();
			HudWeaponSlotHandle2 = CachedPlayerHudController->WidgetSpecialAmmoChangeDelegate.AddUObject(this, &UPlayerHUD::SetHudWeaponSlot2);
		}
		else if (SlotNum == 2)
		{
			HudWeaponSlotHandle3.Reset();
			HudWeaponSlotHandle3 = CachedPlayerHudController->WidgetSpecialAmmoChangeDelegate.AddUObject(this, &UPlayerHUD::SetHudWeaponSlot3);
		}
		break;
	case Impact:
		if (SlotNum == 0)
		{
			HudWeaponSlotHandle1.Reset();
			HudWeaponSlotHandle1 = CachedPlayerHudController->WidgetImpactAmmoChangeDelegate.AddUObject(this, &UPlayerHUD::SetHudWeaponSlot1);
		}
		else if (SlotNum == 1)
		{
			HudWeaponSlotHandle2.Reset();
			HudWeaponSlotHandle2 = CachedPlayerHudController->WidgetImpactAmmoChangeDelegate.AddUObject(this, &UPlayerHUD::SetHudWeaponSlot2);
		}
		else if (SlotNum == 2)
		{
			HudWeaponSlotHandle3.Reset();
			HudWeaponSlotHandle3 = CachedPlayerHudController->WidgetImpactAmmoChangeDelegate.AddUObject(this, &UPlayerHUD::SetHudWeaponSlot3);
		}
		break;
	case HighPower:
		if (SlotNum == 0)
		{
			HudWeaponSlotHandle1.Reset();
			HudWeaponSlotHandle1 = CachedPlayerHudController->WidgetHighPowerAmmoChangeDelegate.AddUObject(this, &UPlayerHUD::SetHudWeaponSlot1);
		}
		else if (SlotNum == 1)
		{
			HudWeaponSlotHandle2.Reset();
			HudWeaponSlotHandle2 = CachedPlayerHudController->WidgetHighPowerAmmoChangeDelegate.AddUObject(this, &UPlayerHUD::SetHudWeaponSlot2);
		}
		else if (SlotNum == 2)
		{
			HudWeaponSlotHandle3.Reset();
			HudWeaponSlotHandle3 = CachedPlayerHudController->WidgetHighPowerAmmoChangeDelegate.AddUObject(this, &UPlayerHUD::SetHudWeaponSlot3);
		}
		break;
	default:
		UE_LOG(LogTemp, Error, TEXT("SlotNum Error in PlayerHud!"));
	}
}

void UPlayerHUD::BindCallbacksToDependencies()
{
	CachedPlayerHudController->WidgetHealthChangeDelegate.AddUObject(this, &UPlayerHUD::SetHealth);
	CachedPlayerHudController->WidgetMaxHealthChangeDelegate.AddUObject(this, &UPlayerHUD::SetMaxHealth);
	CachedPlayerHudController->WidgetShieldChangeDelegate.AddUObject(this, &UPlayerHUD::SetShield);
	CachedPlayerHudController->WidgetMaxShieldChangeDelegate.AddUObject(this, &UPlayerHUD::SetMaxShield);
	CachedPlayerHudController->WidgetAmmoChangeDelegate.AddUObject(this, &UPlayerHUD::SetAmmo);
	CachedPlayerHudController->WidgetManaChangeDelegate.AddUObject(this, &UPlayerHUD::SetManaValue);
	CachedPlayerHudController->WidgetMaxManaChangeDelegate.AddUObject(this, &UPlayerHUD::SetMaxManaValue);
	CachedPlayerHudController->WidgetSpecialResourceChangeDelegate.AddUObject(this, &UPlayerHUD::SetSpecialResourceValue);
	CachedPlayerHudController->WidgetMaxSpecialResourceChangeDelegate.AddUObject(this, &UPlayerHUD::SetMaxSpecialResourceValue);
}

void UPlayerHUD::SetHealth(float NewValue)
{
	Health = NewValue;
	SetHealthBar();
}

void UPlayerHUD::SetMaxHealth(float NewValue)
{
	MaxHealth = NewValue;
	SetHealthBar();
}

void UPlayerHUD::SetShield(float NewValue)
{
	Shield = NewValue;
	SetShieldBar(false);
}

void UPlayerHUD::SetMaxShield(float NewValue)
{
	MaxShield = NewValue;
	SetShieldBar(true);
}

void UPlayerHUD::SetAmmo(float NewValue)
{
	FNumberFormattingOptions CurrentAmmoFormattingOptions;
	CurrentAmmoFormattingOptions.MaximumFractionalDigits = 0;
	CurrentAmmoFormattingOptions.MinimumFractionalDigits = 0;
	CurrentAmmoText->SetText(FText::AsNumber(NewValue, &CurrentAmmoFormattingOptions));
}

void UPlayerHUD::SetLeftAmmo(float NewValue)
{
	FNumberFormattingOptions CurrentAmmoFormattingOptions;
	CurrentAmmoFormattingOptions.MaximumFractionalDigits = 0;
	CurrentAmmoFormattingOptions.MinimumFractionalDigits = 0;
	LeftAmmoText->SetText(FText::AsNumber(NewValue, &CurrentAmmoFormattingOptions));
}

void UPlayerHUD::SetHealthBar()
{
	if (MaxHealth != 0)
	{
		HealthProgressBar->SetPercent(Health/MaxHealth);
	}
}

void UPlayerHUD::SetShieldBar(bool bIsMaxChanged)
{
	if (bIsMaxChanged)
	{
		if (MaxShield != 0)
		{
			ShieldImageContainer->ClearChildren();
			
			int BoxNum = MaxShield / 20;
			for (int i = 0; i < BoxNum; i++)
			{
				UImage* NewShieldImage = NewObject<UImage>(this);
				NewShieldImage->SetBrushFromTexture(ShieldImage);
				NewShieldImage->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.95f));
				UHorizontalBoxSlot* ImageSlot = ShieldImageContainer->AddChildToHorizontalBox(NewShieldImage);
				ImageSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
				ImageSlot->SetHorizontalAlignment(HAlign_Fill);
				ImageSlot->SetVerticalAlignment(VAlign_Fill);

				USpacer* NewSpacer = NewObject<USpacer>(this);
				NewSpacer->SetSize(FVector2D(40.f / BoxNum, 1.f));
				UHorizontalBoxSlot* SpacerSlot = ShieldImageContainer->AddChildToHorizontalBox(NewSpacer);
				SpacerSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
				SpacerSlot->SetHorizontalAlignment(HAlign_Fill);
				SpacerSlot->SetVerticalAlignment(VAlign_Fill);
			}
			UImage* NewShieldImage = NewObject<UImage>(this);
			NewShieldImage->SetBrushFromTexture(ShieldImage);
			NewShieldImage->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.95f));
			UHorizontalBoxSlot* ImageSlot = ShieldImageContainer->AddChildToHorizontalBox(NewShieldImage);
			ImageSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
			ImageSlot->SetHorizontalAlignment(HAlign_Fill);
			ImageSlot->SetVerticalAlignment(VAlign_Fill);
		}
	}

	if (MaxShield != 0)
	{
		float ShieldPercent = Shield / MaxShield;

		if (ShieldRetainerMID)
		{
			ShieldRetainerMID->SetScalarParameterValue(FName("FillPercent"), ShieldPercent);
		}
	}
}

void UPlayerHUD::SetManaValue(float InCurrentMana)
{
	CurrentMana = InCurrentMana;
	SetManaProgressBar();

	SetManaLackImage();
}

void UPlayerHUD::SetMaxManaValue(float InMaxMana)
{
	MaxMana = InMaxMana;
	SetManaProgressBar();
}

void UPlayerHUD::SetManaProgressBar()
{
	if (MaxMana <= 0)
	{
		return;
	}

	ManaProgressBar->SetPercent(CurrentMana/MaxMana);
}

void UPlayerHUD::SetSpecialResourceValue(float InCurrentSpecialResource)
{
	CurrentSpecialResource = InCurrentSpecialResource;
	SetSpecialResourceProgressBar();

	SetSpecialResourceLackImage();
}

void UPlayerHUD::SetMaxSpecialResourceValue(float InMaxSpecialResource)
{
	MaxSpecialResource = InMaxSpecialResource;
	SetSpecialResourceProgressBar();
}

void UPlayerHUD::SetSpecialResourceProgressBar()
{
	if (MaxSpecialResource <= 0)
	{
		return;
	}

	SpecialResourceProgressBar->SetPercent(CurrentSpecialResource/MaxSpecialResource);
}

void UPlayerHUD::SetSkillCoolDownImage(FGameplayTag CooldownTag)
{
	float OutTimeRemaining = 0.f;
	float OutTotalTime = 1.f;

	if (AFDPlayerState* PlayerState = GetOwningPlayer()->GetPlayerState<AFDPlayerState>())
	{
		if (UAbilitySystemComponent* ASC = PlayerState->GetAbilitySystemComponent())
		{
			UFDGameplayStatic::TryGetCooldownTimeRemaining(ASC, CooldownTag, OutTimeRemaining, OutTotalTime);
		}
		else
		{
			return;
		}
	}
	else
	{
		return;
	}
	
	if (CooldownTag.MatchesTag(FGameplayTag::RequestGameplayTag("Data.Skill.Cooldown.1")))
	{
		FirstSkillInfo->SetSkillCoolTimeImage(OutTimeRemaining, OutTotalTime);
	}
	else if (CooldownTag.MatchesTag(FGameplayTag::RequestGameplayTag("Data.Skill.Cooldown.2")))
	{
		SecondSkillInfo->SetSkillCoolTimeImage(OutTimeRemaining, OutTotalTime);
	}
	else if (CooldownTag.MatchesTag(FGameplayTag::RequestGameplayTag("Data.Skill.Cooldown.3")))
	{
		ThirdSkillInfo->SetSkillCoolTimeImage(OutTimeRemaining, OutTotalTime);
	}
	else if (CooldownTag.MatchesTag(FGameplayTag::RequestGameplayTag("Data.Skill.Cooldown.4")))
	{
		FourthSkillInfo->SetSkillCoolTimeImage(OutTimeRemaining, OutTotalTime);
	}
}

void UPlayerHUD::SetManaLackImage()
{
	if (AFDPlayerState* PlayerState = GetOwningPlayer()->GetPlayerState<AFDPlayerState>())
	{
		if (UAbilitySystemComponent* ASC = PlayerState->GetAbilitySystemComponent())
		{
			float SkillCostDownRate = ASC->GetNumericAttribute(UPlayerAttributeSet::GetSkillCostAttribute());
			float SkillCost2 = ASC->GetNumericAttribute(UPlayerAttributeSet::GetSkillCost2Attribute());

			if (CurrentMana >= SkillCost2*SkillCostDownRate)
			{
				SecondSkillInfo->SetManaLackImage(true);
			}
			else
			{
				SecondSkillInfo->SetManaLackImage(false);
			}
		}
	}
}

void UPlayerHUD::SetSpecialResourceLackImage()
{
	if (AFDPlayerState* PlayerState = GetOwningPlayer()->GetPlayerState<AFDPlayerState>())
	{
		if (UAbilitySystemComponent* ASC = PlayerState->GetAbilitySystemComponent())
		{
			float SkillCostDownRate = ASC->GetNumericAttribute(UPlayerAttributeSet::GetSkillCostAttribute());
			float SkillCost1 = ASC->GetNumericAttribute(UPlayerAttributeSet::GetSkillCost1Attribute());
			float SkillCost3 = ASC->GetNumericAttribute(UPlayerAttributeSet::GetSkillCost3Attribute());
			float SkillCost4 = ASC->GetNumericAttribute(UPlayerAttributeSet::GetSkillCost4Attribute());

			if (CurrentSpecialResource >= SkillCost1*SkillCostDownRate)
			{
				FirstSkillInfo->SetManaLackImage(true);
			}
			else
			{
				FirstSkillInfo->SetManaLackImage(false);
			}

			if (CurrentSpecialResource >= SkillCost3*SkillCostDownRate)
			{
				ThirdSkillInfo->SetManaLackImage(true);
			}
			else
			{
				ThirdSkillInfo->SetManaLackImage(false);
			}

			if (CurrentSpecialResource >= SkillCost4*SkillCostDownRate)
			{
				FourthSkillInfo->SetManaLackImage(true);
			}
			else
			{
				FourthSkillInfo->SetManaLackImage(false);
			}
		}
	}
}
