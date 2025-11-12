// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameframework/FDLobbyGameMode.h"

#include "Character/Player/FDPlayerCharacter.h"
#include "Controller/FDPlayerController.h"
#include "Gameframework/FDPlayerState.h"
#include "GameFramework/GameStateBase.h"


AFDLobbyGameMode::AFDLobbyGameMode()
{
	bUseSeamlessTravel = true;
	bBlockAccess = false;
}

void AFDLobbyGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId,
	FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	if (GetNumPlayers() >= RequiredPlayerCount)
	{
		ErrorMessage = TEXT("ServerFull");
		return;
	}
}

void AFDLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	NewPlayer->GetPlayerState<AFDPlayerState>()->SetIsLobby(true);

	if (GetNumPlayers() > BattleParty.Num())
	{
		bBlockAccess = false;
	}

	UpdatePlayersReadyInfo();
}

void AFDLobbyGameMode::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();
}

void AFDLobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	BattleParty.Remove(Cast<APlayerController>(Exiting));

	TryStartMatch();
}

void AFDLobbyGameMode::GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList)
{
	Super::GetSeamlessTravelActorList(bToTransition, ActorList);

	// UWorld* World = GetWorld();
	// if (World)
	// {
	// 	for (APlayerState* PS : World->GetGameState()->PlayerArray)
	// 	{
	// 		ActorList.AddUnique(PS->GetPlayerController()->GetCharacter());
	// 	}
	// }
}

void AFDLobbyGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);

	C->GetPlayerState<AFDPlayerState>()->SetIsLobby(true);

	UpdatePlayersReadyInfo();
}

void AFDLobbyGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	// RestartPlayer(NewPlayer);
}

UClass* AFDLobbyGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	return PlayerCharacterClass ? *PlayerCharacterClass : nullptr;
}

void AFDLobbyGameMode::RollForParty(APlayerController* NewPlayer)
{
	if (bBlockAccess && GetNumPlayers() != 1)
	{
		return;
	}
	
	if (BattleParty.Find(NewPlayer) == INDEX_NONE)
	{
		BattleParty.AddUnique(NewPlayer);
	}
	else
	{
		BattleParty.Remove(NewPlayer);
	}
	
	TryStartMatch();
}


void AFDLobbyGameMode::TryStartMatch()
{
	int32 ReadyPlayerNum = BattleParty.Num();
	int32 NumPlayers = GetNumPlayers();

	// Everyone in this lobby map should be ready
	if (ReadyPlayerNum == NumPlayers)
	{
		bBlockAccess = true;

		MovePlayersToBattle();
		// GetWorldTimerManager().SetTimer(StartTimerHandle, this, &AFDLobbyGameMode::MovePlayersToBattle, StartDelay, false);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(StartTimerHandle);
		StartTimerHandle.Invalidate();
	}

	UpdatePlayersReadyInfo();
}

void AFDLobbyGameMode::MovePlayersToBattle()
{
	UWorld* World = GetWorld();
	if (World)
	{
		for (APlayerController* ControllerIt : BattleParty)
		{
			if (AFDPlayerCharacter* Char = Cast<AFDPlayerCharacter>(ControllerIt->GetCharacter()))
			{
				SetDisableInput(ControllerIt);
				ControllerIt->GetPlayerState<AFDPlayerState>()->CaptureInventories(Char);
				GetWorldTimerManager().ClearTimer(Cast<AFDPlayerController>(ControllerIt)->LobbyInitTimerHandle);
				Cast<AFDPlayerController>(ControllerIt)->LobbyInitTimerHandle.Invalidate();
			}
		}
		
		GetWorldTimerManager().SetTimer(StartTimerHandle, [this, World]()
		{
			World->ServerTravel(TEXT("/Game/Levels/Excutioner_Map"), false, false);
		},
		StartDelay,
		false);
		
		// World->ServerTravel(TEXT("/Game/Levels/Excutioner_Map?listen"), true, false);
		// World->SeamlessTravel(TEXT("/Game/Levels/Excutioner_Map?listen"));
	}
}

void AFDLobbyGameMode::SetDisableInput(APlayerController* InController)
{
	if (InController)
	{
		Cast<AFDPlayerController>(InController)->SetDisableInput();
	}
}

void AFDLobbyGameMode::UpdatePlayersReadyInfo()
{
	int ReadyNum = BattleParty.Num();
	int MaxNum = GetNumPlayers();
	for (TObjectPtr<APlayerState> PlayerStateIt : GetWorld()->GetGameState()->PlayerArray)
	{
		if (AFDPlayerController* PlayerController = Cast<AFDPlayerController>(PlayerStateIt->GetPlayerController()))
		{
			if (BattleParty.Find(PlayerStateIt->GetPlayerController()) == INDEX_NONE)
			{
				PlayerController->SetBattleReady(false, ReadyNum, MaxNum);
			}
			else
			{
				PlayerController->SetBattleReady(true, ReadyNum, MaxNum);
			}
		}
	}
}
