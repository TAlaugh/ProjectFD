// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/InventoryMainWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/ScaleBoxSlot.h"
#include "Components/VerticalBox.h"
#include "Controller/FDPlayerController.h"
#include "DataAssets/Widget/InventorySlotTextures.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Equipment/EquipmentManagerComponent.h"
#include "Inventory/InventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Widget/Inventory/Slot/WeaponInventoryWidget.h"
#include "Widget/Inventory/Slot/WeaponSlotWidget.h"
#include "Widget/Inventory/Slot/Module/ModuleInventoryWidget.h"
#include "Widget/WidgetController/InventoryWidgetController.h"


void UInventoryMainWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	check(InventorySlotTextures);

	if (GetWorld()->GetFirstPlayerController())
	{
		// 액션 바인딩
		if (UEnhancedInputComponent* InputComp = Cast<UEnhancedInputComponent>(GetWorld()->GetFirstPlayerController()->InputComponent))
		{
			InputComp->BindAction(IA_CloseInventory, ETriggerEvent::Started, this, &UInventoryMainWidget::OnCloseInventory);
			InputComp->BindAction(IA_Escape, ETriggerEvent::Started, this, &UInventoryMainWidget::OnEscapeWidget);
		}
	}

	InventoryButton->OnClicked.AddDynamic(this, &UInventoryMainWidget::OnMenuInventoryButtonClicked);
	WeaponBookButton->OnClicked.AddDynamic(this, &UInventoryMainWidget::OnWeaponBookButtonClicked);
	PlayerModuleButton->OnClicked.AddDynamic(this, &UInventoryMainWidget::OnPlayerModuleButtonClicked);
}

void UInventoryMainWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	OnWidgetControllerSet();

	SetPreviewImageAlignment(ImageAlignment::Middle);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = CachedPlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		Subsystem->ClearAllMappings();
		Subsystem->AddMappingContext(InventoryInputMappingContext, 10);
	}
	
	CachedInventoryWidgetController->InventoryEntryDelegate.AddDynamic(this, &UInventoryMainWidget::CreateEquipmentList);
	CachedInventoryWidgetController->PlayerModuleInventoryEntryDelegate.AddDynamic(this, &UInventoryMainWidget::CreatePlayerModuleList);

	WeaponInventoryWidget = CreateWidget<UWeaponInventoryWidget>(this, WeaponInventoryClass);
	if (WeaponInventoryWidget)
	{
		InventoryWrapHorizontalBox->AddChildToHorizontalBox(WeaponInventoryWidget);
		WeaponInventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
		WeaponInventoryWidget->WeaponInventoryDelegate.AddDynamic(this, &UInventoryMainWidget::OnWeaponEquip);
	}

	InventoryWrapHorizontalBox->AddChildToHorizontalBox(MainLeftVerticalBox);
	
	for (int i = 0; i < 3; i++)
	{
		if (UWeaponSlotWidget* NewWidget = CreateWidget<UWeaponSlotWidget, UInventoryMainWidget*>(this, WeaponSlotClass))
		{
			// WeaponVerticalBox->AddChildToVerticalBox(NewWidget);
			WeaponVerticalBox->AddChildToVerticalBox(NewWidget);
			NewWidget->WeaponOrderChangeDelegate.BindUObject(this, &UInventoryMainWidget::OnWeaponOrderChange);
			NewWidget->WeaponEquipDelegate.BindUObject(this, &UInventoryMainWidget::OnWeaponDropEquip);
			NewWidget->WeaponSlotDelegate.AddDynamic(this, &UInventoryMainWidget::OnWeaponSlotClicked);
			NewWidget->WeaponSlotRightClickDelegate.AddDynamic(this, &UInventoryMainWidget::OnWeaponSlotRightClicked);
			NewWidget->InitSlotNum(i);
		}
	}

	CreateEquipmentSlots();

	PlayerInfoVerticalBox->SetVisibility(ESlateVisibility::Visible);
}

void UInventoryMainWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = CachedPlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		Subsystem->RemoveMappingContext(InventoryInputMappingContext);
	}

	// if (EquipmentEntryDelegateHandle.IsValid())
	// {
	// 	CachedEquipmentComponent->EquipmentList.EquipmentEntryDelegate.Remove(EquipmentEntryDelegateHandle);
	// }

	// CachedPlayerController->CreatePlayerHudWidget();
}

void UInventoryMainWidget::OnWidgetControllerSet()
{
	Super::OnWidgetControllerSet();

	checkf(WidgetController, TEXT("InventoryMainWidget : WidgetController not set"));
	
	CachedInventoryWidgetController = Cast<UInventoryWidgetController>(WidgetController);
	CachedPlayerController = Cast<AFDPlayerController>(CachedInventoryWidgetController->GetOwningActor());
	CachedInventoryComponent = CachedPlayerController->GetInventoryComponent_Implementation();
	CachedEquipmentComponent = CachedPlayerController->GetEquipmentComponent();
}

void UInventoryMainWidget::SetPreviewImage(UTextureRenderTarget2D* RTTexture)
{
	PreviewCharacterImage->SetBrushResourceObject(RTTexture);
}

void UInventoryMainWidget::CreateEquipmentList(const FFDInventoryEntry& Entry, float FireTime)
{
	// 여기서 중복으로?
	if (Entry.ItemTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon")))
	{
		if (WeaponInventoryWidget)
		{
			if (Entry.Quantity <= 0)
			{
				WeaponInventoryWidget->DestroySmallWeaponSlot(Entry.ItemID);
			}
			else
			{
				WeaponInventoryWidget->CreateSmallWeaponSlot(Entry, InventorySlotTextures, CachedInventoryComponent->GetItemDefinitionByTag(Entry.ItemTag), FireTime);
			}
		}
	}
}

void UInventoryMainWidget::CreatePlayerModuleList(const FFDInventoryEntry& Entry)
{
	if (Entry.ItemTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Module")))
	{
		if (ModuleInventoryWidget && ModuleInventoryWidget->IsInViewport())
		{
			if (Entry.Quantity == 0)
			{
				ModuleInventoryWidget->DestroyUnEquippedModuleSlot(Entry.ItemID);
			}
			else
			{
				ModuleInventoryWidget->CreateUnEquippedModuleSlot(Entry, InventorySlotTextures, CachedInventoryComponent->GetItemDefinitionByTag(Entry.ItemTag));
			}
		}
		// ModuleInventoryWidget->SetAppliedEffectBox();
	}
}

void UInventoryMainWidget::CreateEquipmentSlots()
{
	for (const FFDEquipmentEntry& EquipmentEntry : CachedEquipmentComponent->EquipmentList.GetWeaponEntries())
	{
		if (EquipmentEntry.EntryTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon")))
		{
			UWeaponSlotWidget* CachedWeaponSlot = Cast<UWeaponSlotWidget>(WeaponVerticalBox->GetChildAt(EquipmentEntry.SlotNum));
			CachedWeaponSlot->SetSlotInfo(EquipmentEntry, InventorySlotTextures, CachedInventoryComponent->GetItemDefinitionByTag(EquipmentEntry.EntryTag));
		}
	}
}

void UInventoryMainWidget::ShowWeaponInventory(bool bShow)
{
	if (bShow)
	{
		EquipmentEntryDelegateHandle = CachedEquipmentComponent->EquipmentList.EquipmentEntryDelegate.AddLambda(
			[this](const FFDEquipmentEntry& Entry)
			{
				if (this->WeaponVerticalBox)
				{
					if (Entry.EntryTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon")))
					{
						if (Entry.SlotNum > 2 || Entry.SlotNum < 0)
						{
							UE_LOG(LogTemp, Error, TEXT("SlotNum Error"));
							return;
						}
						if (this && this->IsInViewport())
						{
							// When moving single Weapon Erase PreSlot
							for (int i = 0; i < 3; i++)
							{
								if (Entry.ItemID == Cast<UWeaponSlotWidget>(this->WeaponVerticalBox->GetChildAt(i))->GetItemID() && Entry.SlotNum != i)
								{
									Cast<UWeaponSlotWidget>(this->WeaponVerticalBox->GetChildAt(i))->SetSlotDefault();
								}
							}
							Cast<UWeaponSlotWidget>(this->WeaponVerticalBox->GetChildAt(Entry.SlotNum))->SetSlotInfo(Entry, this->InventorySlotTextures, this->CachedInventoryComponent->GetItemDefinitionByTag(Entry.EntryTag));
						}
					}
				}
			});
		EquipmentEntryDelegateHandle = CachedEquipmentComponent->EquipmentList.UnEquipmentEntryDelegate.AddLambda(
			[this](const FFDEquipmentEntry& Entry)
			{
				if (this->WeaponVerticalBox)
				{
					if (Entry.EntryTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon")))
					{
						if (Entry.SlotNum > 2 || Entry.SlotNum < 0)
						{
							UE_LOG(LogTemp, Error, TEXT("SlotNum Error"));
							return;
						}
						if (this && this->IsInViewport())
						{
							Cast<UWeaponSlotWidget>(this->WeaponVerticalBox->GetChildAt(Entry.SlotNum))->SetSlotDefault();
						}
					}
				}
			});

		SetPreviewImageAlignment(ImageAlignment::Left);
		WeaponInventoryWidget->ClearSmallWeaponSlots();
		CachedInventoryWidgetController->BroadcastWeaponInventoryInitialValues();
		PlayerInfoVerticalBox->SetVisibility(ESlateVisibility::Collapsed);
		ETCVerticalBox->SetVisibility(ESlateVisibility::Collapsed);
		WeaponInventoryWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		if (EquipmentEntryDelegateHandle.IsValid())
		{
			CachedEquipmentComponent->EquipmentList.EquipmentEntryDelegate.Remove(EquipmentEntryDelegateHandle);
		}

		SetPreviewImageAlignment(ImageAlignment::Middle);
		PlayerInfoVerticalBox->SetVisibility(ESlateVisibility::Visible);
		ETCVerticalBox->SetVisibility(ESlateVisibility::Visible);
		// WeaponInventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
		WeaponInventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UInventoryMainWidget::ShowPlayerModuleInventory(bool bShow)
{
	if (bShow)
	{
		ModuleInventoryWidget->ClearAllUnEquippedSlots();
		CachedInventoryWidgetController->BroadcastPlayerModuleInventoryInitialValues();
		
		SetPreviewImageAlignment(ImageAlignment::Hide);
		PlayerInfoVerticalBox->SetVisibility(ESlateVisibility::Collapsed);
		ETCVerticalBox->SetVisibility(ESlateVisibility::Collapsed);

		ModuleInventoryWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		SetPreviewImageAlignment(ImageAlignment::Middle);
		PlayerInfoVerticalBox->SetVisibility(ESlateVisibility::Visible);
		ETCVerticalBox->SetVisibility(ESlateVisibility::Visible);

		ModuleInventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

UInventoryComponent* UInventoryMainWidget::GetInventoryComponent()
{
	return CachedInventoryComponent;
}

void UInventoryMainWidget::SetUnEquippedWeaponSlots(int SlotNum)
{
	if (WeaponVerticalBox)
	{
		if (SlotNum > 2 || SlotNum < 0)
		{
			UE_LOG(LogTemp, Error, TEXT("SlotNum Error"));
			return;
		}
		if (IsInViewport())
		{
			Cast<UWeaponSlotWidget>(WeaponVerticalBox->GetChildAt(SlotNum))->SetSlotDefault();
		}
	}
}

void UInventoryMainWidget::SetUnEquippedModuleSlots(int SlotNum)
{
	if (ModuleInventoryWidget)
	{
		if (SlotNum > 11 || SlotNum < 0)
		{
			UE_LOG(LogTemp, Error, TEXT("SlotNum Error"));
			return;
		}
		if (IsInViewport())
		{
			ModuleInventoryWidget->SetEquippedModuleSlotToDefault(SlotNum);
		}
	}
}

void UInventoryMainWidget::SetModuleEffectSlot()
{
	ModuleInventoryWidget->SetAppliedEffectBox();
}

void UInventoryMainWidget::SetPreviewImageAlignment(ImageAlignment Location)
{
	if (Location == ImageAlignment::Middle)
	{
		if (PreviewCharacterImage)
		{
			if (UScaleBoxSlot* ScaleSlot = Cast<UScaleBoxSlot>(PreviewCharacterImage->Slot))
			{
				PreviewCharacterImage->SetVisibility(ESlateVisibility::Visible);
				ScaleSlot->SetHorizontalAlignment(HAlign_Center);
			}
		}
	}
	else if (Location == ImageAlignment::Left)
	{
		if (PreviewCharacterImage)
		{
			if (UScaleBoxSlot* ScaleSlot = Cast<UScaleBoxSlot>(PreviewCharacterImage->Slot))
			{
				PreviewCharacterImage->SetVisibility(ESlateVisibility::Visible);
				ScaleSlot->SetHorizontalAlignment(HAlign_Left);
			}
		}
	}
	else if (Location == Right)
	{
		if (PreviewCharacterImage)
		{
			if (UScaleBoxSlot* ScaleSlot = Cast<UScaleBoxSlot>(PreviewCharacterImage->Slot))
			{
				PreviewCharacterImage->SetVisibility(ESlateVisibility::Visible);
				ScaleSlot->SetHorizontalAlignment(HAlign_Right);
			}
		}
	}
	else if (Location == Hide)
	{
		if (PreviewCharacterImage)
		{
			PreviewCharacterImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UInventoryMainWidget::OnMenuInventoryButtonClicked()
{
	UE_LOG(LogTemp, Warning,TEXT("Inventory Button Clicked!!"));
}

void UInventoryMainWidget::OnWeaponBookButtonClicked()
{
	OnWeaponSlotClicked(FFDEquipmentEntry());
}

void UInventoryMainWidget::OnPlayerModuleButtonClicked()
{
	if (!ModuleInventoryWidget || not ModuleInventoryWidget->IsInViewport())
	{
		check(ModuleInventoryClass);
		ModuleInventoryWidget = CreateWidget<UModuleInventoryWidget>(this, ModuleInventoryClass);
		ModuleInventoryWidget->SetInventoryComponent(CachedInventoryComponent);
		ModuleInventoryWidget->SetEquipmentComponent(CachedEquipmentComponent);
		ModuleInventoryWidget->SetInventoryMainWidget(this);
		ModuleInventoryWidget->AddToViewport(1);
		ShowPlayerModuleInventory(true);
	}
	else
	{
		ShowPlayerModuleInventory(true);
	}
}

void UInventoryMainWidget::OnWeaponSlotClicked(const FFDEquipmentEntry& EquipmentEntry)
{
	// Move to Weapon Inventory
 	if (WeaponInventoryWidget->GetVisibility() != ESlateVisibility::Visible)
 	{
 		ShowWeaponInventory(true);
 		
 		if (!EquipmentEntry.EntryTag.IsValid())
 		{
 			// ShowWeaponInventory(true);
 		}
 		else
 		{
 			// Cast<UWeaponSlotWidget>(WeaponVerticalBox->GetChildAt(EquipmentEntry.SlotNum))->SetSelectedImage(true);
 		}
 	}
 	// Change to this Weapon
 	else
 	{
 		if (EquipmentEntry.EntryTag.IsValid() && EquipmentEntry.SlotNum != CachedEquipmentComponent->GetActiveWeaponSlot())
 		{
 			// Cast<UWeaponSlotWidget>(WeaponVerticalBox->GetChildAt(CachedEquipmentComponent->GetActiveWeaponSlot()))->SetSelectedImage(false);
 			UGameplayStatics::PlaySound2D(GetWorld(), EquipSFX, 0.9f, 1.0f);
 			CachedEquipmentComponent->ChangeWeapon(EquipmentEntry.SlotNum);
 			
 			// Cast<UWeaponSlotWidget>(WeaponVerticalBox->GetChildAt(EquipmentEntry.SlotNum))->SetSelectedImage(true);
 		}
 	}
}

void UInventoryMainWidget::OnWeaponSlotRightClicked(const FFDEquipmentEntry& EquipmentEntry)
{
	if (EquipmentEntry.EntryTag.IsValid())
	{
		CachedEquipmentComponent->UnequipItem(EquipmentEntry);
	}
}

void UInventoryMainWidget::OnWeaponEquip(const FFDInventoryEntry& Entry)
{
	CachedInventoryComponent->UseItem(Entry, 1, CachedEquipmentComponent->GetActiveWeaponSlot());
}

void UInventoryMainWidget::OnWeaponDropEquip(const FFDInventoryEntry& Entry, int DropSlotNum)
{
	CachedInventoryComponent->UseItem(Entry, 1, DropSlotNum);
}

// 위젯 수동 정렬 왜 해야하는가
void UInventoryMainWidget::OnWeaponOrderChange(const FFDEquipmentEntry& EquipmentEntry, int DropSlotNum)
{
	CachedEquipmentComponent->ChangeOrderWeapon(EquipmentEntry.SlotNum, DropSlotNum);
}

void UInventoryMainWidget::OnCloseInventory()
{
	if (ModuleInventoryWidget)
	{
		ModuleInventoryWidget->RemoveFromParent();
		ModuleInventoryWidget = nullptr;
	}
	RemoveFromParent();
	CachedPlayerController->CreatePlayerHudWidget();
}

void UInventoryMainWidget::OnEscapeWidget()
{
	// Is Main Inventory?
	if (PlayerInfoVerticalBox->GetVisibility() != ESlateVisibility::Visible)
	{
		// Not just WeaponInventory
		if (WeaponInventoryWidget && (WeaponInventoryWidget->GetVisibility() == ESlateVisibility::Visible || WeaponInventoryWidget->IsInViewport()))
		{
			ShowWeaponInventory(false);
		}
		else if (ModuleInventoryWidget && (ModuleInventoryWidget->GetVisibility() == ESlateVisibility::Visible || ModuleInventoryWidget->IsInViewport()))
		{
			ShowPlayerModuleInventory(false);
		}
	}
	else
	{
		RemoveFromParent();
		CachedPlayerController->CreatePlayerHudWidget();
	}
}
