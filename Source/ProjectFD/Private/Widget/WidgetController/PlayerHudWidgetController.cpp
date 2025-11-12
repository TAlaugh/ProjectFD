// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/WidgetController/PlayerHudWidgetController.h"

#include "Character/Player/FDPlayerCharacter.h"
#include "Controller/FDPlayerController.h"
#include "Equipment/EquipmentManagerComponent.h"
#include "Gameframework/FDPlayerState.h"
#include "GAS/Player/PlayerAttributeSet.h"


AActor* UPlayerHudWidgetController::GetOwningActor()
{
	if (OwningActor)
	{
		return OwningActor;
	}

	return nullptr;
}

void UPlayerHudWidgetController::SetOwningActor(AActor* InOwner)
{
	OwningActor = InOwner;
}

void UPlayerHudWidgetController::BindCallbacksToDependencies()
{
	AFDPlayerController* OwningController = Cast<AFDPlayerController>(OwningActor);

	UEquipmentManagerComponent* CachedEquipmentComponent = Cast<AFDPlayerController>(GetOwningActor())->GetEquipmentComponent();

	if (CachedEquipmentComponent)
	{
		CachedEquipmentComponent->EquipmentList.EquipmentEntryDelegate.AddLambda(
			[this] (const FFDEquipmentEntry& Entry)
			{
				// if (Entry.EntryTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon")))
				// {
				// 	HudEquipmentEntryDelegate.Broadcast(Entry);
				// }
				HudEquipmentEntryResetDelegate.Broadcast();
				
				UEquipmentManagerComponent* CachedEquipmentComponent = Cast<AFDPlayerController>(GetOwningActor())->GetEquipmentComponent();

				if (CachedEquipmentComponent)
				{
					for (const FFDEquipmentEntry& Entry : CachedEquipmentComponent->EquipmentList.GetWeaponEntries())
					{
						if (Entry.EntryTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon")))
						{
							HudEquipmentEntryDelegate.Broadcast(Entry);
						}
					}
				}
			});
	}

	if (OwningController)
	{
		if (AFDPlayerState* PlayerState = OwningController->GetPlayerState<AFDPlayerState>())
		{
			if (UAbilitySystemComponent* ASC = PlayerState->GetAbilitySystemComponent())
			{
				if (const UPlayerAttributeSet* ConstAttrSet = Cast<UPlayerAttributeSet>(ASC->GetAttributeSet(UPlayerAttributeSet::StaticClass())))
				{
					ASC->GetGameplayAttributeValueChangeDelegate(ConstAttrSet->GetHealthAttribute()).AddLambda(
						[this] (const FOnAttributeChangeData& Data)
						{
							WidgetHealthChangeDelegate.Broadcast(Data.NewValue);
						});
					ASC->GetGameplayAttributeValueChangeDelegate(ConstAttrSet->GetMaxHealthAttribute()).AddLambda(
						[this] (const FOnAttributeChangeData& Data)
						{
							WidgetMaxHealthChangeDelegate.Broadcast(Data.NewValue);
						});
					ASC->GetGameplayAttributeValueChangeDelegate(ConstAttrSet->GetShieldAttribute()).AddLambda(
						[this] (const FOnAttributeChangeData& Data)
						{
							WidgetShieldChangeDelegate.Broadcast(Data.NewValue);
						});
					ASC->GetGameplayAttributeValueChangeDelegate(ConstAttrSet->GetMaxShieldAttribute()).AddLambda(
						[this] (const FOnAttributeChangeData& Data)
						{
							WidgetMaxShieldChangeDelegate.Broadcast(Data.NewValue);
						});
					ASC->GetGameplayAttributeValueChangeDelegate(ConstAttrSet->GetAmmoAttribute()).AddLambda(
						[this] (const FOnAttributeChangeData& Data)
						{
							WidgetAmmoChangeDelegate.Broadcast(Data.NewValue);
						});
					ASC->GetGameplayAttributeValueChangeDelegate(ConstAttrSet->GetGeneralAmmoAttribute()).AddLambda(
						[this] (const FOnAttributeChangeData& Data)
						{
							WidgetGeneralAmmoChangeDelegate.Broadcast(Data.NewValue);
						});
					ASC->GetGameplayAttributeValueChangeDelegate(ConstAttrSet->GetSpecialAmmoAttribute()).AddLambda(
						[this] (const FOnAttributeChangeData& Data)
						{
							WidgetSpecialAmmoChangeDelegate.Broadcast(Data.NewValue);
						});
					ASC->GetGameplayAttributeValueChangeDelegate(ConstAttrSet->GetImpactAmmoAttribute()).AddLambda(
						[this] (const FOnAttributeChangeData& Data)
						{
							WidgetImpactAmmoChangeDelegate.Broadcast(Data.NewValue);
						});
					ASC->GetGameplayAttributeValueChangeDelegate(ConstAttrSet->GetHighPowerAmmoAttribute()).AddLambda(
						[this] (const FOnAttributeChangeData& Data)
						{
							WidgetHighPowerAmmoChangeDelegate.Broadcast(Data.NewValue);
						});
					ASC->GetGameplayAttributeValueChangeDelegate(ConstAttrSet->GetManaAttribute()).AddLambda(
						[this] (const FOnAttributeChangeData& Data)
						{
							WidgetManaChangeDelegate.Broadcast(Data.NewValue);
						});
					ASC->GetGameplayAttributeValueChangeDelegate(ConstAttrSet->GetMaxManaAttribute()).AddLambda(
						[this] (const FOnAttributeChangeData& Data)
						{
							WidgetMaxManaChangeDelegate.Broadcast(Data.NewValue);
						});
					ASC->GetGameplayAttributeValueChangeDelegate(ConstAttrSet->GetSpecialResourceAttribute()).AddLambda(
						[this] (const FOnAttributeChangeData& Data)
						{
							WidgetSpecialResourceChangeDelegate.Broadcast(Data.NewValue);
						});
					ASC->GetGameplayAttributeValueChangeDelegate(ConstAttrSet->GetMaxSpecialResourceAttribute()).AddLambda(
						[this] (const FOnAttributeChangeData& Data)
						{
							WidgetMaxSpecialResourceChangeDelegate.Broadcast(Data.NewValue);
						});
				}
			}
		}
	}
}

// Init???
void UPlayerHudWidgetController::BroadcastInitialValues()
{
	UEquipmentManagerComponent* CachedEquipmentComponent = Cast<AFDPlayerController>(GetOwningActor())->GetEquipmentComponent();

	if (CachedEquipmentComponent)
	{
		for (const FFDEquipmentEntry& Entry : CachedEquipmentComponent->EquipmentList.GetWeaponEntries())
		{
			if (Entry.EntryTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon")))
			{
				HudEquipmentEntryDelegate.Broadcast(Entry);
			}
		}
	}
	
	
	AFDPlayerController* OwningController = Cast<AFDPlayerController>(OwningActor);

	if (OwningController)
	{
		if (AFDPlayerState* PlayerState = OwningController->GetPlayerState<AFDPlayerState>())
		{
			if (UAbilitySystemComponent* ASC = PlayerState->GetAbilitySystemComponent())
			{
				WidgetHealthChangeDelegate.Broadcast(ASC->GetNumericAttribute(UPlayerAttributeSet::GetHealthAttribute()));
				WidgetMaxHealthChangeDelegate.Broadcast(ASC->GetNumericAttribute(UPlayerAttributeSet::GetMaxHealthAttribute()));
				WidgetShieldChangeDelegate.Broadcast(ASC->GetNumericAttribute(UPlayerAttributeSet::GetShieldAttribute()));
				WidgetMaxShieldChangeDelegate.Broadcast(ASC->GetNumericAttribute(UPlayerAttributeSet::GetMaxShieldAttribute()));
				WidgetAmmoChangeDelegate.Broadcast(ASC->GetNumericAttribute(UPlayerAttributeSet::GetAmmoAttribute()));
				WidgetGeneralAmmoChangeDelegate.Broadcast(ASC->GetNumericAttribute(UPlayerAttributeSet::GetGeneralAmmoAttribute()));
				WidgetSpecialAmmoChangeDelegate.Broadcast(ASC->GetNumericAttribute(UPlayerAttributeSet::GetSpecialAmmoAttribute()));
				WidgetImpactAmmoChangeDelegate.Broadcast(ASC->GetNumericAttribute(UPlayerAttributeSet::GetImpactAmmoAttribute()));
				WidgetHighPowerAmmoChangeDelegate.Broadcast(ASC->GetNumericAttribute(UPlayerAttributeSet::GetHighPowerAmmoAttribute()));
				WidgetManaChangeDelegate.Broadcast(ASC->GetNumericAttribute(UPlayerAttributeSet::GetManaAttribute()));
				WidgetMaxManaChangeDelegate.Broadcast(ASC->GetNumericAttribute(UPlayerAttributeSet::GetMaxManaAttribute()));
				WidgetSpecialResourceChangeDelegate.Broadcast(ASC->GetNumericAttribute(UPlayerAttributeSet::GetSpecialResourceAttribute()));
				WidgetMaxSpecialResourceChangeDelegate.Broadcast(ASC->GetNumericAttribute(UPlayerAttributeSet::GetMaxSpecialResourceAttribute()));
			}
		}
	}
}
