// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/Slot/Module/ModuleInventoryWidget.h"


#include "ScalableFloat.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/MenuAnchor.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WrapBox.h"
#include "Equipment/ModuleDefinition.h"
#include "Gameframework/FDPlayerState.h"
#include "GAS/Player/PlayerAttributeSet.h"
#include "Inventory/ItemTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Widget/Inventory/InventoryMainWidget.h"
#include "Widget/Inventory/Slot/FilterOptionWidget.h"
#include "Widget/Inventory/Slot/Module/ModuleEquippedSlotWidget.h"
#include "Widget/Inventory/Slot/Module/ModuleSlotWidget.h"
#include "Widget/WidgetController/InventoryWidgetController.h"


void UModuleInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	OrderButton->OnClicked.AddDynamic(this, &UModuleInventoryWidget::OnOrderButtonClicked);
	SortButton->OnClicked.AddDynamic(this, &UModuleInventoryWidget::OnSortButtonClicked);
	SortMenuAnchor->OnGetUserMenuContentEvent.BindUFunction(this, FName("OnSortMenuAnchorActivate"));
}

void UModuleInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	EquippedModuleContainer->ClearChildren();

	const TArray<FModuleSlotInfo>& CachedModuleSlotInfos = CachedEquipmentComponent->GetModuleSlotInfos();
	
	for (int i = 0; i < 12; i++)
	{
		UModuleEquippedSlotWidget* NewEquippedSlot = CreateWidget<UModuleEquippedSlotWidget>(this, ModuleEquippedSlotClass);
		NewEquippedSlot->InitSlotNum(i);
		NewEquippedSlot->SetDecoImage(i);
		if (!CachedModuleSlotInfos[i].Sockets.IsEmpty())
		{
			NewEquippedSlot->SetSocketImage(CachedModuleSlotInfos[i].Sockets.First(), InventorySlotTextures);
		}
		NewEquippedSlot->ModuleEquipDelegate.BindUObject(this, &UModuleInventoryWidget::OnModuleEquip);
		NewEquippedSlot->ModuleUnEquipDelegate.BindUObject(this, &UModuleInventoryWidget::OnModuleUnEquip);
		NewEquippedSlot->ModuleOrderChangeDelegate.BindUObject(this, &UModuleInventoryWidget::OnModuleOrderChange);
		EquippedModuleContainer->AddChildToWrapBox(NewEquippedSlot);
	}

	SetEquippedModuleSlot();

	if (CachedEquipmentComponent)
	{
		EquipmentEntryDelegateHandle = CachedEquipmentComponent->EquipmentList.EquipmentEntryDelegate.AddLambda(
				[this](const FFDEquipmentEntry& Entry)
				{
					if (Entry.EntryTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Module.Player")) && EquippedModuleContainer)
					{
						if (Entry.SlotNum > 12 || Entry.SlotNum < 0)
						{
							UE_LOG(LogTemp, Error, TEXT("SlotNum Error"));
							return;
						}
						if (this->IsInViewport())
						{
							EModuleCapacity ModuleCapacity = EModuleCapacity::None;
							if (!CachedEquipmentComponent->GetModuleSlotInfos()[Entry.SlotNum].Sockets.IsEmpty())
							{
								if (Entry.SlotTag.MatchesTag(CachedEquipmentComponent->GetModuleSlotInfos()[Entry.SlotNum].Sockets.First()))
								{
									ModuleCapacity = EModuleCapacity::Half;
								}
								else
								{
									ModuleCapacity = EModuleCapacity::Double;
								}
							}
							if (Entry.EntryTag.IsValid())
							{
								UGameplayStatics::PlaySound2D(GetWorld(), EquipModuleSFX, 0.9f, 1.0f);
							}
							Cast<UModuleEquippedSlotWidget>(EquippedModuleContainer->GetChildAt(Entry.SlotNum))->SetSlotInfo(Entry, InventorySlotTextures, CachedInventoryComponent->GetItemDefinitionByTag(Entry.EntryTag), ModuleCapacity);
							SetAppliedEffectBox();
						}
					}
				});
	}

	if (AFDPlayerState* PlayerState = GetOwningPlayer()->GetPlayerState<AFDPlayerState>())
	{
		if (UAbilitySystemComponent* ASC = PlayerState->GetAbilitySystemComponent())
		{
			SetMaxModuleCapacity(ASC->GetNumericAttribute(UPlayerAttributeSet::GetMaxModuleCapacityAttribute()));
			SetModuleCapacity(ASC->GetNumericAttribute(UPlayerAttributeSet::GetModuleCapacityAttribute()));
		}
	}

	SetOrderImage();
	SetAppliedEffectBox();
}

void UModuleInventoryWidget::NativeDestruct()
{
	Super::NativeDestruct();

	// CachedEquipmentComponent->EquipmentList.EquipmentEntryDelegate.Remove(EquipmentEntryDelegateHandle);
}

void UModuleInventoryWidget::SetEquippedModuleSlot()
{
	for (const FFDEquipmentEntry& EquipmentEntry : CachedEquipmentComponent->EquipmentList.GetWeaponEntries())
	{
		if (EquipmentEntry.EntryTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Module.Player")))
		{
			EModuleCapacity ModuleCapacityInfo = EModuleCapacity::None;
			const UModuleDefinition* ModuleCDO = GetDefault<UModuleDefinition>(CachedInventoryComponent->GetItemDefinitionByTag(EquipmentEntry.EntryTag).ModuleProps.ModuleClass);

			if (CachedEquipmentComponent->GetModuleSlotInfos()[EquipmentEntry.SlotNum].Sockets.IsEmpty())
			{
				// Doing nothing
			}
			else if (CachedEquipmentComponent->GetModuleSlotInfos()[EquipmentEntry.SlotNum].Sockets.First().MatchesTag(ModuleCDO->SocketTypeTag))
			{
				ModuleCapacityInfo = EModuleCapacity::Half;
			}
			else
			{
				ModuleCapacityInfo = EModuleCapacity::Double;
			}
			UModuleEquippedSlotWidget* CachedWeaponSlot = Cast<UModuleEquippedSlotWidget>(EquippedModuleContainer->GetChildAt(EquipmentEntry.SlotNum));
			CachedWeaponSlot->SetSlotInfo(EquipmentEntry, InventorySlotTextures, CachedInventoryComponent->GetItemDefinitionByTag(EquipmentEntry.EntryTag), ModuleCapacityInfo);
		}
	}
}

void UModuleInventoryWidget::SetEquippedModuleSlotToDefault(int SlotNum)
{
	for (const FFDEquipmentEntry& EquipmentEntry : CachedEquipmentComponent->EquipmentList.GetWeaponEntries())
	{
		if (EquipmentEntry.EntryTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Module.Player")) && EquipmentEntry.SlotNum == SlotNum)
		{
			UModuleEquippedSlotWidget* CachedModuleSlot = Cast<UModuleEquippedSlotWidget>(EquippedModuleContainer->GetChildAt(EquipmentEntry.SlotNum));
			CachedModuleSlot->SetDefaultSlotInfo();
		}
	}
}

void UModuleInventoryWidget::CreateUnEquippedModuleSlot(const FFDInventoryEntry& Entry, UInventorySlotTextures* TextureMap, FMasterItemDefinition ItemDefinition)
{
	for (UWidget* WidgetIt : UnEquippedModuleContainer->GetAllChildren())
	{
		UModuleSlotWidget* ModuleSlotWidget = Cast<UModuleSlotWidget>(WidgetIt);
		if (ModuleSlotWidget->GetItemID() == Entry.ItemID)
		{
			ModuleSlotWidget->SetSlotInfo(Entry, TextureMap, ItemDefinition);
			return;
		}
	}
	if (UModuleSlotWidget* NewModuleSlot = CreateWidget<UModuleSlotWidget>(this, ModuleSlotClass))
	{
		UnEquippedModuleContainer->AddChildToWrapBox(NewModuleSlot);
		NewModuleSlot->SetSlotInfo(Entry, TextureMap, ItemDefinition);
		SortUnEquippedModules(Name);
	}
}

void UModuleInventoryWidget::DestroyUnEquippedModuleSlot(int64 ItemID)
{
	for (UWidget* WidgetIt : UnEquippedModuleContainer->GetAllChildren())
	{
		UModuleSlotWidget* ModuleSlotWidget = Cast<UModuleSlotWidget>(WidgetIt);
		if (ModuleSlotWidget->GetItemID() == ItemID)
		{
			ModuleSlotWidget->RemoveFromParent();
			break;
		}
	}
}

void UModuleInventoryWidget::ClearAllUnEquippedSlots()
{
	UnEquippedModuleContainer->ClearChildren();
}

void UModuleInventoryWidget::SetInventoryMainWidget(UInventoryMainWidget* InInventoryMainWidget)
{
	CachedInventoryMainWidget = InInventoryMainWidget;

	BindCallbacksToDependencies();
}

void UModuleInventoryWidget::SetInventoryComponent(UInventoryComponent* InInventoryComponent)
{
	CachedInventoryComponent = InInventoryComponent;
}

void UModuleInventoryWidget::SetEquipmentComponent(UEquipmentManagerComponent* InEquipmentComponent)
{
	CachedEquipmentComponent = InEquipmentComponent;
}

void UModuleInventoryWidget::OnModuleEquip(const FFDInventoryEntry& Entry, int SlotNum)
{
	CachedInventoryComponent->UseItem(Entry, 1, SlotNum);
}

void UModuleInventoryWidget::OnModuleUnEquip(const FFDEquipmentEntry& Entry)
{
	CachedEquipmentComponent->UnequipItem(Entry);
}

void UModuleInventoryWidget::OnModuleOrderChange(const FFDEquipmentEntry& SourceEntry, int DestinationSlotNum)
{
	CachedEquipmentComponent->ChangeOrderModule(SourceEntry, DestinationSlotNum);
}

void UModuleInventoryWidget::OnOrderButtonClicked()
{
	bIsAscending = not bIsAscending;
	SortUnEquippedModules(CurrentSortType);
	SetOrderImage();
}

void UModuleInventoryWidget::OnSortButtonClicked()
{
	if (SortMenuAnchor)
	{
		SortMenuAnchor->ToggleOpen(true);
	}
}

UWidget* UModuleInventoryWidget::OnSortMenuAnchorActivate()
{
	UBorder* SortMenuBorder = NewObject<UBorder>(this);
	FSlateBrush TransparentBrush;
	TransparentBrush.TintColor = FSlateColor(FLinearColor(1.f, 1.f, 1.f, 0.f)); // 완전 투명
	SortMenuBorder->SetBrush(TransparentBrush);
	
	UVerticalBox* SortMenuContainer = NewObject<UVerticalBox>(this);
	SortMenuBorder->AddChild(SortMenuContainer);
	
	TArray<FText> MenuNames = {FText::FromString(TEXT("등급")), FText::FromString(TEXT("수용량")), FText::FromString(TEXT("수량")),
								FText::FromString(TEXT("강화")), FText::FromString(TEXT("소켓")), FText::FromString(TEXT("이름"))};
	TArray<EModuleSortType> MenuTypes = {Rarity, Capacity, Quantity, Enchant, Socket, Name};
	
	for (int i = 0; i < MenuNames.Num(); i++)
	{
		UFilterOptionWidget* NewFilterOption = CreateWidget<UFilterOptionWidget>(this, FilterOptionSlotClass);
		NewFilterOption->SetOptionNameText(FText::FromString(FString(TEXT("정렬 기준: ")) + MenuNames[i].ToString()));
		NewFilterOption->SetOptionNum(MenuTypes[i]);
		NewFilterOption->FilterOptionButtonDelegate.AddUObject(this, &UModuleInventoryWidget::SortUnEquippedModules);
		// if (UCanvasPanelSlot* NewCanvasSlot = Cast<UCanvasPanelSlot>(NewFilterOption->Slot))
		// {
		// 	NewCanvasSlot->SetZOrder(11);
		// }
		
		SortMenuContainer->AddChild(NewFilterOption);
	}
	
	return SortMenuBorder;
}

void UModuleInventoryWidget::SetAppliedEffectBox()
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,
		[this] ()
		{
			AppliedStatsScrollBox->ClearChildren();
			AppliedAbilityScrollBox->ClearChildren();
			
			TMap<FName,float> AppliedStatEffectMap = TMap<FName,float>();
			TArray<FText> AppliedAbilities = TArray<FText>();
			
			for (const FFDEquipmentEntry& EntryIt : CachedEquipmentComponent->EquipmentList.GetWeaponEntries())
			{
				if (not EntryIt.EntryTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Module.Player")))
				{
					continue;
				}

				// Sum of all Module Effects
				for (const FEquipmentStatEffectGroup& StatEffect : EntryIt.EffectPackage.StatEffects)
				{
					// GameplayEffect
					UGameplayEffect* GECDO = StatEffect.EffectClass.LoadSynchronous()->GetDefaultObject<UGameplayEffect>();
					FGameplayEffectContextHandle ContextHandle;
					FGameplayEffectSpec Spec(GECDO, ContextHandle, StatEffect.CurrentValue);
					for (const FGameplayModifierInfo& Mod : GECDO->Modifiers)
					{
						float StatEffectValue = 0.f;

						if (Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(StatEffect.CurrentValue, StatEffectValue))
						{
							switch (Mod.ModifierOp)
							{
							case EGameplayModOp::Additive:
								break;
								
							case EGameplayModOp::Multiplicitive:
								StatEffectValue -= 1.f;
								break;
								
							default:
								UE_LOG(LogTemp, Error, TEXT("GameplayEffect Has Other ModifierOperations!"));
							}
							if (AppliedStatEffectMap.Find(*StatEffect.AttributeName.ToString()))
							{
								AppliedStatEffectMap[*StatEffect.AttributeName.ToString()] += StatEffectValue;
							}
							else
							{
								AppliedStatEffectMap.Add(*StatEffect.AttributeName.ToString(), StatEffectValue);						
							}
						}
					}
				}
				// // Ability
				// for (;;)
				// {
				// 	
				// }
			}
			for (const TPair<FName,float>& Pair : AppliedStatEffectMap)
			{
				UOverlay* NewOverlay = NewObject<UOverlay>(this);
				UImage* NewImage = NewObject<UImage>(this);
				NewImage->SetColorAndOpacity(FLinearColor(0.5f, 0.5f, 0.5f, 0.3f));
				UOverlaySlot* NewOverlaySlot = NewOverlay->AddChildToOverlay(NewImage);
				NewOverlaySlot->SetPadding(FMargin(0.f, 5.f));
				NewOverlaySlot->SetHorizontalAlignment(HAlign_Fill);
				NewOverlaySlot->SetVerticalAlignment(VAlign_Fill);
				UHorizontalBox* NewStatBox = NewObject<UHorizontalBox>(this);
				UOverlaySlot* AnotherOverlaySlot = NewOverlay->AddChildToOverlay(NewStatBox);
				AnotherOverlaySlot->SetPadding(FMargin(5.f, 0.f));
				AnotherOverlaySlot->SetHorizontalAlignment(HAlign_Fill);
				AnotherOverlaySlot->SetVerticalAlignment(VAlign_Fill);
				
				UTextBlock* AttributeName = NewObject<UTextBlock>(this);
				AttributeName->SetJustification(ETextJustify::Left);
				FSlateFontInfo NameFontInfo = AttributeName->GetFont();
				NameFontInfo.Size = 16;
				AttributeName->SetFont(NameFontInfo);
				AttributeName->SetText(FText::FromName(Pair.Key));
				
				UTextBlock* AttributeValue = NewObject<UTextBlock>(this);
				AttributeValue->SetJustification(ETextJustify::Right);
				FSlateFontInfo ValueFontInfo = AttributeValue->GetFont();
				ValueFontInfo.Size = 16;
				AttributeValue->SetFont(ValueFontInfo);
				FNumberFormattingOptions FormatOptions;
				FormatOptions.MaximumFractionalDigits = 1;
				FormatOptions.MinimumFractionalDigits = 0;
				AttributeValue->SetText(FText::Format(FText::FromString("{0} {1}"), FText::AsNumber(Pair.Value * 100, &FormatOptions), FText::FromString(TEXT("%"))));
				
				UHorizontalBoxSlot* AttributeNameSlot = NewStatBox->AddChildToHorizontalBox(AttributeName);
				AttributeNameSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
				AttributeNameSlot->SetHorizontalAlignment(HAlign_Fill);
				AttributeNameSlot->SetVerticalAlignment(VAlign_Center);
				
				UHorizontalBoxSlot* AttributeValueSlot = NewStatBox->AddChildToHorizontalBox(AttributeValue);
				AttributeValueSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
				AttributeValueSlot->SetHorizontalAlignment(HAlign_Fill);
				AttributeValueSlot->SetVerticalAlignment(VAlign_Center);
				
				// AppliedStatsScrollBox->AddChild(NewStatBox);
				AppliedStatsScrollBox->AddChild(NewOverlay);
			}			
		},
		0.2f,
		false);
	
}

void UModuleInventoryWidget::SetModuleCapacity(float CurrentCapacity)
{
	ModuleCapacity = CurrentCapacity;
	SetModuleCostInfo();
}

void UModuleInventoryWidget::SetMaxModuleCapacity(float MaxCapacity)
{
	MaxModuleCapacity = MaxCapacity;
	SetModuleCostInfo();
}

void UModuleInventoryWidget::SetModuleCostInfo()
{
	FString CostInfoStr = FString::FromInt(ModuleCapacity) + TEXT(" / ") + FString::FromInt(MaxModuleCapacity);
	ModuleCostInfoText->SetText(FText::FromString(CostInfoStr));

	if (MaxModuleCapacity > 0)
	{
		ModuleCostProgressBar->SetPercent(ModuleCapacity/MaxModuleCapacity);
	}
}

void UModuleInventoryWidget::SetOrderImage()
{
	if (bIsAscending)
	{
		OrderAscendingImage->SetVisibility(ESlateVisibility::Visible);
		OrderDescendingImage->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		OrderAscendingImage->SetVisibility(ESlateVisibility::Hidden);
		OrderDescendingImage->SetVisibility(ESlateVisibility::Visible);
	}
}

void UModuleInventoryWidget::SortUnEquippedModules(int SortType)
{
	if (SortMenuAnchor)
	{
		SortMenuAnchor->Close();
	}
	
	CurrentSortType = static_cast<EModuleSortType>(SortType);

	switch (CurrentSortType)
	{
	case Newest:
		SortTypeText->SetText(FText::FromString(TEXT("정렬 기준: 최신")));
		break;
	case Rarity:
		SortTypeText->SetText(FText::FromString(TEXT("정렬 기준: 등급")));
		break;
	case Capacity:
		SortTypeText->SetText(FText::FromString(TEXT("정렬 기준: 수용량")));
		break;
	case Quantity:
		SortTypeText->SetText(FText::FromString(TEXT("정렬 기준: 수량")));
		break;
	case Enchant:
		SortTypeText->SetText(FText::FromString(TEXT("정렬 기준: 강화")));
		break;
	case Socket:
		SortTypeText->SetText(FText::FromString(TEXT("정렬 기준: 소켓")));
		break;
	case Name:
		SortTypeText->SetText(FText::FromString(TEXT("정렬 기준: 이름")));
		break;
	}
	
	TArray<UModuleSlotWidget*> OrderedSlots;
	for (UWidget* SlotIt : UnEquippedModuleContainer->GetAllChildren())
	{
		OrderedSlots.Add(Cast<UModuleSlotWidget>(SlotIt));
	}
	OrderedSlots.Sort([this](UModuleSlotWidget& A, UModuleSlotWidget& B)
	{
		FMasterItemDefinition DefinitionA = CachedInventoryComponent->GetItemDefinitionByTag(A.GetCachedInventoryEntry().ItemTag);
		FMasterItemDefinition DefinitionB = CachedInventoryComponent->GetItemDefinitionByTag(B.GetCachedInventoryEntry().ItemTag);
		
		switch (CurrentSortType)
		{
		case Newest:	return false;
		case Rarity:
			if (GetRarityPriority(DefinitionA.RarityTag) == GetRarityPriority(DefinitionB.RarityTag))
			{
				return A.GetCachedInventoryEntry().ItemName.ToString() < B.GetCachedInventoryEntry().ItemName.ToString();
			}
			if (bIsAscending)
			{
				return GetRarityPriority(DefinitionA.RarityTag) < GetRarityPriority(DefinitionB.RarityTag);
			}
			return not (GetRarityPriority(DefinitionA.RarityTag) < GetRarityPriority(DefinitionB.RarityTag));
		case Capacity:
			if (FCString::Atoi(*A.GetCapacityText().ToString()) == FCString::Atoi(*B.GetCapacityText().ToString()))
			{
				return A.GetCachedInventoryEntry().ItemName.ToString() < B.GetCachedInventoryEntry().ItemName.ToString();
			}
			if (bIsAscending)
			{
				return FCString::Atoi(*A.GetCapacityText().ToString()) < FCString::Atoi(*B.GetCapacityText().ToString());
			}
			return not (FCString::Atoi(*A.GetCapacityText().ToString()) < FCString::Atoi(*B.GetCapacityText().ToString()));
		case Quantity:
			if (A.GetCachedInventoryEntry().Quantity == B.GetCachedInventoryEntry().Quantity)
			{
				return A.GetCachedInventoryEntry().ItemName.ToString() < B.GetCachedInventoryEntry().ItemName.ToString();
			}
			if (bIsAscending)
			{
				return A.GetCachedInventoryEntry().Quantity < B.GetCachedInventoryEntry().Quantity;
			}
			return not (A.GetCachedInventoryEntry().Quantity < B.GetCachedInventoryEntry().Quantity);
		case Enchant:
			if (A.GetCachedInventoryEntry().Level == B.GetCachedInventoryEntry().Level)
			{
				return A.GetCachedInventoryEntry().ItemName.ToString() < B.GetCachedInventoryEntry().ItemName.ToString();
			}
			if (bIsAscending)
			{
				return A.GetCachedInventoryEntry().Level < B.GetCachedInventoryEntry().Level;
			}
			return not (A.GetCachedInventoryEntry().Level < B.GetCachedInventoryEntry().Level);
		case Socket:
			if (true)
			{
				const UModuleDefinition* ModuleCDOA = GetDefault<UModuleDefinition>(DefinitionA.ModuleProps.ModuleClass.Get());
				const UModuleDefinition* ModuleCDOB = GetDefault<UModuleDefinition>(DefinitionB.ModuleProps.ModuleClass.Get());
				if (ModuleCDOA->SocketTypeTag.ToString() == ModuleCDOB->SocketTypeTag.ToString())
				{
					return A.GetCachedInventoryEntry().ItemName.ToString() < B.GetCachedInventoryEntry().ItemName.ToString();
				}
				if (bIsAscending)
				{
					return ModuleCDOA->SocketTypeTag.ToString() < ModuleCDOB->SocketTypeTag.ToString();
				}
				return not (ModuleCDOA->SocketTypeTag.ToString() < ModuleCDOB->SocketTypeTag.ToString());
			}
		case Name:
			if (A.GetCachedInventoryEntry().ItemName.ToString() == B.GetCachedInventoryEntry().ItemName.ToString())
			{
				return A.GetCachedInventoryEntry().Level < B.GetCachedInventoryEntry().Level;
			}
			if (bIsAscending)
			{
				return A.GetCachedInventoryEntry().ItemName.ToString() < B.GetCachedInventoryEntry().ItemName.ToString();
			}
			return not (A.GetCachedInventoryEntry().ItemName.ToString() < B.GetCachedInventoryEntry().ItemName.ToString());
		default:		return false;
		}
	});
	UnEquippedModuleContainer->ClearChildren();
	for (UModuleSlotWidget* SlotIt : OrderedSlots)
	{
		UnEquippedModuleContainer->AddChild(SlotIt);
	}
}

void UModuleInventoryWidget::BindCallbacksToDependencies()
{
	if (CachedInventoryMainWidget)
	{
		CachedInventoryMainWidget->CachedInventoryWidgetController->ModuleCapacityChangeDelegate.AddUObject(this, &UModuleInventoryWidget::SetModuleCapacity);
		CachedInventoryMainWidget->CachedInventoryWidgetController->MaxModuleCapacityChangeDelegate.AddUObject(this, &UModuleInventoryWidget::SetMaxModuleCapacity);
	}
}

int32 UModuleInventoryWidget::GetRarityPriority(FGameplayTag RarityTag)
{
	if (RarityTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("UI.Equipment.Tier.Transcendent")))
	{
		return 3;
	}
	else if (RarityTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("UI.Equipment.Tier.Ultimate")))
	{
		return 2;
	}
	else if (RarityTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("UI.Equipment.Tier.Rare")))
	{
		return 1;
	}
	else if (RarityTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("UI.Equipment.Tier.Normal")))
	{
		return 0;
	}
	else
	{
		return -1;
	}
}
