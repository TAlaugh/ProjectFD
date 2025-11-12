// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FDLobbyGameMode.generated.h"

class AFDPlayerController;
/**
 * 
 */
UCLASS()
class PROJECTFD_API AFDLobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	AFDLobbyGameMode();

	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void PostSeamlessTravel() override;
	virtual void Logout(AController* Exiting) override;
	virtual void GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList) override;
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	UFUNCTION()
	void RollForParty(APlayerController* NewPlayer);

	void UpdatePlayersReadyInfo();
	
protected:
	
	void TryStartMatch();
	void MovePlayersToBattle();

	void SetDisableInput(APlayerController* InController);

	UPROPERTY()
	TArray<APlayerController*> BattleParty = TArray<APlayerController*>();

	UPROPERTY(EditDefaultsOnly, Category="Match")
	int32 RequiredPlayerCount = 3;

	UPROPERTY(EditDefaultsOnly, Category="Match")
	float StartDelay = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Value", meta = (AllowPrivateAccess=true))
	TSubclassOf<APawn> PlayerCharacterClass;

	FTimerHandle StartTimerHandle;

	bool bBlockAccess = false;
	
	
};
