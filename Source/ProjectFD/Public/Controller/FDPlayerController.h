// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/InventoryInterface.h"
#include "FDPlayerController.generated.h"

class UBossStatusHUD;
class AFDBossCharacter;
class ULobbyWidget;
struct FGameplayTag;
class UPlayerHUD;
struct FFDEquipmentEntry;
class UInventoryMainWidget;
class UInputAction;
class UInputMappingContext;
class UPlayerHudWidgetController;
class UEquipmentManagerComponent;
class UBaseWidget;
class UInventoryWidgetController;
class UInventoryComponent;
class AFDPlayerCharacter;

/**
 * 
 */
UCLASS()
class PROJECTFD_API AFDPlayerController : public APlayerController, public IAbilitySystemInterface, public IInventoryInterface
{
	GENERATED_BODY()
	
public:
	AFDPlayerController();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UInventoryComponent* GetInventoryComponent_Implementation() override;
	virtual void SetInventoryComponent(TArray<FFDInventoryEntry> InEntries) override;

	virtual void GetSeamlessTravelActorList(bool bToEntry, TArray<class AActor*>& ActorList) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	// 서버에서만 호출
	virtual void OnPossess(APawn* InPawn) override;
	// 클라이언트에서만 호출 ( 소유권 증명 ) - 서버에 연결되었을때 호출 | 애그노리지 퍼세션
	virtual void AcknowledgePossession(class APawn* P) override;

	UFUNCTION(Client, Reliable)
	void SetDisableInput();

	UEquipmentManagerComponent* GetEquipmentComponent();

	UPlayerHudWidgetController* GetPlayerHudWidgetController();
	UInventoryWidgetController* GetInventoryWidgetController();

	UFUNCTION(BlueprintCallable)
	void CreatePlayerHudWidget();

	UFUNCTION(BlueprintCallable)
	void CreateInventoryWidget();

	UFUNCTION(BlueprintCallable)
	void RemovePlayerHudWidget();

	UFUNCTION(Server, Unreliable)
	void GetReadyForBattle();

	void StartSkillCoolDownByTag(FGameplayTag CoolDownTag);
	void RefreshUnEquippedWeaponSlot(int SlotNum);
	void RefreshUnEquippedModuleSlot(int SlotNum);
	void RefreshModuleEffectSlot();
	void CallWeaponSlotToDefault(FFDEquipmentEntry UnEquippedEntry);

	UFUNCTION(Client, Reliable)
	void SetBattleReady(bool bIsReady, int ReadyNum, int MaxNum);

	UFUNCTION(Server, Reliable)
	void RefreshBattleReady();
	
#pragma region BossWidget
	void ShowBossStatusHUD(AFDBossCharacter* Boss);
	
	UFUNCTION()
	void HideBossStatusHUD();
	
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UBossStatusHUD> BossStatusHUDClass;

	UPROPERTY()
	UBossStatusHUD* BossStatusHUD;

	UFUNCTION(Client, Reliable)
	void Client_FadeOut();

	UFUNCTION(Client, Reliable)
	void Client_FadeIn();

	UFUNCTION(Client, Reliable)
	void Client_SuccessAnim();

	UFUNCTION(Client, Reliable)
	void Client_FailedAnim();
#pragma endregion
	
private:
	
	UPROPERTY()
	AFDPlayerCharacter* FDPlayerCharacter;

	UFUNCTION()
	void SetPlayerInputMappingContext();

public:
	
	FTimerHandle LobbyInitTimerHandle;

protected:

	virtual void BeginPlay() override;

private:

#pragma region WidgetAssets

	UPROPERTY()
	TObjectPtr<UPlayerHudWidgetController> PlayerHudWidgetController;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Widgets")
	TSubclassOf<UPlayerHudWidgetController> PlayerHudWidgetControllerClass;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TObjectPtr<UPlayerHUD> PlayerHudWidget;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Widgets")
	TSubclassOf<UPlayerHUD> PlayerHudWidgetClass;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TObjectPtr<ULobbyWidget> PlayerLobbyHudWidget;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Widgets")
	TSubclassOf<ULobbyWidget> PlayerLobbyHudWidgetClass;	

	UPROPERTY()
	TObjectPtr<UInventoryWidgetController> InventoryWidgetController;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Widgets")
	TSubclassOf<UInventoryWidgetController> InventoryWidgetControllerClass;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TObjectPtr<UInventoryMainWidget> InventoryWidget;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Widgets")
	TSubclassOf<UInventoryMainWidget> InventoryWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Input")
	TObjectPtr<UInputMappingContext> PlayerInputMappingContext;

#pragma endregion
};
