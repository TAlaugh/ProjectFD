// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/FDPlayerController.h"
#include "GameFramework/PlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Character/Monster/Boss/FDBossCharacter.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "Character/Player/FDPreviewCharacter.h"
#include "Equipment/EquipmentManagerComponent.h"
#include "Gameframework/FDLobbyGameMode.h"
#include "Gameframework/FDPlayerState.h"
#include "GAS/Monster/EnemyAttributeSet.h"
#include "Widget/WidgetController/InventoryWidgetController.h"
#include "Widget/BaseWidget.h"
#include "Widget/LobbyWidget.h"
#include "Widget/HUD/BossStatusHUD.h"
#include "Widget/HUD/PlayerHUD.h"
#include "Widget/Inventory/InventoryMainWidget.h"
#include "Widget/WidgetController/PlayerHudWidgetController.h"


AFDPlayerController::AFDPlayerController()
{
	FDPlayerCharacter = Cast<AFDPlayerCharacter>(GetCharacter());
}

void AFDPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (not HasAuthority())
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle,
			[this] ()
			{
				// if (UUserWidget* Widget = CreateWidget<UBaseWidget>(this, PlayerHudWidgetClass))
				// {
				// 	this->PlayerHudWidget = Cast<UPlayerHUD>(Widget);
				// 	this->PlayerHudWidget->SetWidgetController(GetPlayerHudWidgetController());
				// 	this->PlayerHudWidget->AddToViewport();
				// 	this->PlayerHudWidgetController->BroadcastInitialValues();
				//
				// 	FInputModeGameOnly InputMode;
				// 	SetInputMode(InputMode);
				// 	this->bShowMouseCursor = false;
				// }
				CreatePlayerHudWidget();
				GetCharacter()->EnableInput(this);
			},
			5.f,
			false);
	}

	// if (UUserWidget* Widget = CreateWidget<UBaseWidget>(this, PlayerHudWidgetClass))
	// {
	// 	PlayerHudWidget = Cast<UBaseWidget>(Widget);
	// 	PlayerHudWidget->SetWidgetController(GetPlayerHudWidgetController());
	// 	PlayerHudWidgetController->BroadcastInitialValues();
	// 	PlayerHudWidget->AddToViewport();
	//
	// 	FInputModeGameOnly InputMode;
	// 	SetInputMode(InputMode);
	// 	bShowMouseCursor = false;
	// }

	BossStatusHUD = nullptr;
}

UAbilitySystemComponent* AFDPlayerController::GetAbilitySystemComponent() const
{
	// if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(GetPawn()))
	// {
	// 	return ASCInterface->GetAbilitySystemComponent();
	// }

	// return UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());

	if (FDPlayerCharacter)
	{
		return FDPlayerCharacter->GetAbilitySystemComponent();
	}

	return nullptr;
}

UInventoryComponent* AFDPlayerController::GetInventoryComponent_Implementation()
{
	return Cast<AFDPlayerCharacter>(GetCharacter())->GetInventoryComponent_Implementation();
}

void AFDPlayerController::SetInventoryComponent(TArray<FFDInventoryEntry> InEntries)
{
	// Does not need yet
}

void AFDPlayerController::GetSeamlessTravelActorList(bool bToEntry, TArray<class AActor*>& ActorList)
{
	Super::GetSeamlessTravelActorList(bToEntry, ActorList);
}

void AFDPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AFDPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!FDPlayerCharacter)
	{
		FDPlayerCharacter = Cast<AFDPlayerCharacter>(InPawn);
	}
}

void AFDPlayerController::AcknowledgePossession(class APawn* P)
{
	Super::AcknowledgePossession(P);

	if (!FDPlayerCharacter)
	{
		FDPlayerCharacter = Cast<AFDPlayerCharacter>(P);
	}

	AFDPlayerState* PS = GetPlayerState<AFDPlayerState>();
	if (PS)
	{
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, P);
	}
}

void AFDPlayerController::SetDisableInput_Implementation()
{
	DisableInput(this);
}

UEquipmentManagerComponent* AFDPlayerController::GetEquipmentComponent()
{
	if (AFDPlayerCharacter* OwningPlayer = Cast<AFDPlayerCharacter>(GetCharacter()))
	{
		return OwningPlayer->GetEquipmentComponent();
	}

	return nullptr;
}

UPlayerHudWidgetController* AFDPlayerController::GetPlayerHudWidgetController()
{	
	if (!IsValid(PlayerHudWidgetController))
	{
		PlayerHudWidgetController = NewObject<UPlayerHudWidgetController>(this, PlayerHudWidgetControllerClass);
		PlayerHudWidgetController->SetOwningActor(this);
		PlayerHudWidgetController->BindCallbacksToDependencies();
	}

	return PlayerHudWidgetController;
}

UInventoryWidgetController* AFDPlayerController::GetInventoryWidgetController()
{
	if (!IsValid(InventoryWidgetController))
	{
		InventoryWidgetController = NewObject<UInventoryWidgetController>(this, InventoryWidgetControllerClass);
		InventoryWidgetController->SetOwningActor(this);
		InventoryWidgetController->BindCallbacksToDependencies();
	}

	return InventoryWidgetController;
}

void AFDPlayerController::CreatePlayerHudWidget()
{
	if (GetPlayerState<AFDPlayerState>()->GetIsLobby())
	{
		if (PlayerLobbyHudWidget)
		{
			RefreshBattleReady();
			PlayerLobbyHudWidget->SetVisibility(ESlateVisibility::Visible);
		}
		else if (ULobbyWidget* LobbyWidget = CreateWidget<ULobbyWidget>(this, PlayerLobbyHudWidgetClass))
		{
			PlayerLobbyHudWidget = LobbyWidget;
			RefreshBattleReady();
			LobbyWidget->AddToViewport();
		}

		FInputModeGameAndUI InputMode;
		SetInputMode(InputMode);
		bShowMouseCursor = true;
	}
	else
	{
		if (PlayerHudWidget)
		{
			PlayerHudWidget->SetVisibility(ESlateVisibility::Visible);
		}
		else if (UUserWidget* Widget = CreateWidget<UBaseWidget>(this, PlayerHudWidgetClass))
		{
			PlayerHudWidget = Cast<UPlayerHUD>(Widget);
			PlayerHudWidget->SetWidgetController(GetPlayerHudWidgetController());
			PlayerHudWidget->AddToViewport();
			PlayerHudWidgetController->BroadcastInitialValues();
		}

		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		bShowMouseCursor = false;
	}

	if (InventoryWidget)
	{
		// InventoryWidget->RemoveFromParent();
		InventoryWidget = nullptr;
	}

	SetPlayerInputMappingContext();
}

void AFDPlayerController::CreateInventoryWidget()
{
	if (PlayerHudWidget && PlayerHudWidget->IsInViewport())
	{
		// RemovePlayerHudWidget();
		PlayerHudWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	if (PlayerLobbyHudWidget && PlayerLobbyHudWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		PlayerLobbyHudWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	if (InventoryWidget && InventoryWidget->IsInViewport())
	{
		return;
	}
	// Authority Check needed if to call on server
	if (UUserWidget* Widget = CreateWidget<UBaseWidget>(this, InventoryWidgetClass))
	{
		InventoryWidget = Cast<UInventoryMainWidget>(Widget);
		InventoryWidget->SetWidgetController(GetInventoryWidgetController());
		InventoryWidget->SetPreviewImage(FDPlayerCharacter->GetPreviewCharacter()->GetRenderTarget2D());
		// InventoryWidget->SetIsFocusable(true);
		// InventoryWidgetController->BroadcastInitialValues();
		
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);
		bShowMouseCursor = true;
		
		InventoryWidget->AddToViewport();
	}
}

void AFDPlayerController::RemovePlayerHudWidget()
{
	PlayerHudWidget->RemoveFromParent();
	PlayerHudWidget = nullptr;
}

void AFDPlayerController::GetReadyForBattle_Implementation()
{
	if (FDPlayerCharacter)
	{
		// GetPlayerState<AFDPlayerState>()->CaptureInventories(FDPlayerCharacter);
		if (AFDLobbyGameMode* LobbyGameMode = Cast<AFDLobbyGameMode>(GetWorld()->GetAuthGameMode()))
		{
			LobbyGameMode->RollForParty(this);
		}
	}
}

void AFDPlayerController::StartSkillCoolDownByTag(FGameplayTag CoolDownTag)
{
	if (CoolDownTag.IsValid())
	{
		PlayerHudWidget->SkillCoolDownStartDelegate.Broadcast(CoolDownTag);
	}
}

void AFDPlayerController::RefreshUnEquippedWeaponSlot(int SlotNum)
{
	if (InventoryWidget)
	{
		InventoryWidget->SetUnEquippedWeaponSlots(SlotNum);
	}
}

void AFDPlayerController::RefreshUnEquippedModuleSlot(int SlotNum)
{
	if (InventoryWidget)
	{
		InventoryWidget->SetUnEquippedModuleSlots(SlotNum);
	}
}

void AFDPlayerController::RefreshModuleEffectSlot()
{
	if (InventoryWidget)
	{
		InventoryWidget->SetModuleEffectSlot();
	}
}

void AFDPlayerController::CallWeaponSlotToDefault(FFDEquipmentEntry UnEquippedEntry)
{
	if (PlayerHudWidget)
	{
		PlayerHudWidget->SetWeaponInfoToDefault(UnEquippedEntry);
	}
}

void AFDPlayerController::RefreshBattleReady_Implementation()
{
	if (AFDLobbyGameMode* LobbyGM = GetWorld()->GetAuthGameMode<AFDLobbyGameMode>())
	{
		LobbyGM->UpdatePlayersReadyInfo();
	}
}

void AFDPlayerController::ShowBossStatusHUD(AFDBossCharacter* Boss)
{
	if (!BossStatusHUDClass || !Boss) return;
	if (!IsLocalController()) return;

	if (!BossStatusHUD)
	{
		BossStatusHUD = CreateWidget<UBossStatusHUD>(this, BossStatusHUDClass);
		if (!BossStatusHUD) return;

		BossStatusHUD->AddToViewport();
	}

	BossStatusHUD->SetBossRef(Boss);
}

void AFDPlayerController::HideBossStatusHUD()
{
	if (BossStatusHUD && BossStatusHUD->IsInViewport())
	{
		BossStatusHUD->RemoveFromParent();
		BossStatusHUD = nullptr;
	}
}

void AFDPlayerController::Client_FadeOut_Implementation()
{
	if (BossStatusHUD)
	{
		BossStatusHUD->FadeOutImage();
	}
}

void AFDPlayerController::Client_FadeIn_Implementation()
{
	if (BossStatusHUD)
	{
		BossStatusHUD->FadeInImage();
	}
}

void AFDPlayerController::Client_SuccessAnim_Implementation()
{
	if (BossStatusHUD)
	{
		BossStatusHUD->SuccessAnimation();
	}
}

void AFDPlayerController::Client_FailedAnim_Implementation()
{
	if (BossStatusHUD)
	{
		BossStatusHUD->FailedAnimation();
	}
}

void AFDPlayerController::SetBattleReady_Implementation(bool bIsReady, int ReadyNum, int MaxNum)
{
	if (PlayerLobbyHudWidget && IsValid(PlayerLobbyHudWidget))
	{
		PlayerLobbyHudWidget->SetButtonInfo(bIsReady, ReadyNum, MaxNum);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(LobbyInitTimerHandle);
		LobbyInitTimerHandle.Invalidate();
		GetWorld()->GetTimerManager().SetTimer(LobbyInitTimerHandle, [this, bIsReady, ReadyNum, MaxNum]()
		{
			if (PlayerLobbyHudWidget)
			{
				PlayerLobbyHudWidget->SetButtonInfo(bIsReady, ReadyNum, MaxNum);
				GetWorldTimerManager().ClearTimer(LobbyInitTimerHandle);
				LobbyInitTimerHandle.Invalidate();
			}
		},
		1.f,
		false);
	}
}

void AFDPlayerController::SetPlayerInputMappingContext()
{
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (InputSubsystem)
	{
		InputSubsystem->RemoveMappingContext(PlayerInputMappingContext);
		InputSubsystem->AddMappingContext(PlayerInputMappingContext, 0);
	}
}
