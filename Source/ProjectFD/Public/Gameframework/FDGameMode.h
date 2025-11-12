// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FDGameMode.generated.h"

class AFDPlayerController;
class AFDBossCharacter;
/**
 * 
 */
UCLASS()
class PROJECTFD_API AFDGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	AFDGameMode();

	virtual void BeginPlay() override;
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void PostSeamlessTravel() override;
	virtual void GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList) override;
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	
protected:

	void MovePlayersToLobby();

private:

	FTimerHandle MovePlayersToLobbyTimerHandle;
	float EndWaitTime = 30.f;

#pragma region Boss
public:
	void CallBossDead();
	
protected:

	void FixupView(APlayerController* PlayerController);

	void SpawnStartWall();
	void SpawnBossExecutioner(int Level);

private:

	void SetDisableInput(APlayerController* InController);

	UPROPERTY(EditDefaultsOnly, Category = "Custom Value", meta = (AllowPrivateAccess=true))
	TSubclassOf<APawn> PlayerCharacterClass;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess=true))
	TSubclassOf<AFDBossCharacter> BossExecutionerClass;

	UPROPERTY()
	AFDBossCharacter* SpawnedBossExecutioner = nullptr;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess=true))
	TSubclassOf<AActor> StartWallClass;

	UPROPERTY()
	AActor* SpawnedStartWall = nullptr;

#pragma endregion

#pragma region Respawn
public:
	UPROPERTY(EditDefaultsOnly, Category="Respawn")
	int32 DefaultSharedRespawnCount = 4;

	void GameOverAndMoveToLobby();
private:
	UPROPERTY(EditDefaultsOnly, Category="Respawn")
	float GameOverTime = 10.f;
#pragma endregion
};
