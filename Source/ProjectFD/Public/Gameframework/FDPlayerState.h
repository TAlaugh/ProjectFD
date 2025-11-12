// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerState.h"
#include "GenericTeamAgentInterface.h"
#include "FDPlayerState.generated.h"

class AFDLobbyGameMode;
class AFDGameMode;
class AFDPlayerCharacter;
struct FFDEquipmentEntry;
struct FFDInventoryEntry;
struct FGameplayTag;
class UFDAbilitySystemComponent;
class UPlayerAbilitySystemComponent;
class UPlayerAttributeSet;
class UAbilitySystemComponent;
/**
 * 
 */


UCLASS()
class PROJECTFD_API AFDPlayerState : public APlayerState, public IAbilitySystemInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
	
public:
	AFDPlayerState();

	virtual void SeamlessTravelTo(class APlayerState* NewPlayerState) override;
	virtual void CopyProperties(APlayerState* PlayerState) override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	void CaptureInventories(AFDPlayerCharacter* InPlayer);
	void RestoreInventories(AFDPlayerCharacter* InPlayer);

	bool GetIsLobby();
	void SetIsLobby(bool InIsLobby);
	
private:

	friend AFDGameMode;
	friend AFDLobbyGameMode;
	
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly,Replicated, Category = "GAS")
	UPlayerAbilitySystemComponent* PSASC;

	UPROPERTY()
	UPlayerAttributeSet* PlayerAttributeSet;

	UPROPERTY()
	TArray<FFDInventoryEntry> CachedInventoryList;

	UPROPERTY()
	TArray<FFDEquipmentEntry> CachedEquipmentList;

	UPROPERTY()
	int PlayerNum;

	UPROPERTY(Replicated)
	bool bIsLobby;

	
public:
	void AddStartupAbilities();

	UPROPERTY(Replicated)
	FGenericTeamId TeamID;

	virtual FGenericTeamId GetGenericTeamId() const override;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

#pragma region DeathCount
public:
	void BindDeathTagListener();
	
	UPROPERTY(EditDefaultsOnly, Category="Respawn|Tags")
	FGameplayTag DeathTag;
protected:
	void OnDeathTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

private:
	bool bBoundDeathTag = false;
	// 한 번의 사망(Death 태그 부여) 당 1회만 차감하기 위한 가드
	bool bCountedThisDeath = false;

#pragma endregion
};
