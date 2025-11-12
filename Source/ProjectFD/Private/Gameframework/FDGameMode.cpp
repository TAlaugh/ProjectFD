// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameframework/FDGameMode.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "AI/AnimNotify/AN_Thunderbolt.h"
#include "Character/Monster/Boss/FDBossCharacter.h"
#include "Character/Monster/MIN/FDMINCharacter.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "Controller/FDPlayerController.h"
#include "Gameframework/FDGameState.h"
#include "Gameframework/FDPlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "GAS/Player/PlayerAbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Math/TransformCalculus3D.h"

AFDGameMode::AFDGameMode()
{
	bStartPlayersAsSpectators = true;
	bUseSeamlessTravel = true;
}

void AFDGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnStartWall();
	
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, [this]()
	{
		SpawnBossExecutioner(0);
		if (SpawnedStartWall)
		{
			SpawnedStartWall->Destroy();
		}
	},
	10.f,
	false);

	if (AFDGameState* GS = GetGameState<AFDGameState>())
	{
		GS->SetSharedRespawns(DefaultSharedRespawnCount);
	}
}

void AFDGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (!NewPlayer) return;
	
	// PlayerState에 팀 ID 설정
	if (AFDPlayerState* PS = NewPlayer->GetPlayerState<AFDPlayerState>())
	{
		PS->SetIsLobby(false);
	}
}

void AFDGameMode::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();
}

void AFDGameMode::GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList)
{
	Super::GetSeamlessTravelActorList(bToTransition, ActorList);
}

void AFDGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);

	C->GetPlayerState<AFDPlayerState>()->SetIsLobby(false);
}

void AFDGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	RestartPlayer(NewPlayer);
	FixupView(NewPlayer);
	
}

UClass* AFDGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	return PlayerCharacterClass ? *PlayerCharacterClass : nullptr;
}

void AFDGameMode::MovePlayersToLobby()
{
	UWorld* World = GetWorld();
	if (World)
	{
		for (TObjectPtr<APlayerState> PlayerStateIt : GetWorld()->GetGameState()->PlayerArray)
		{
			if (APlayerController* ControllerIt = PlayerStateIt->GetPlayerController())
			{
				if (AFDPlayerCharacter* Char = Cast<AFDPlayerCharacter>(ControllerIt->GetCharacter()))
				{
					SetDisableInput(ControllerIt);
					ControllerIt->GetPlayerState<AFDPlayerState>()->CaptureInventories(Char);
				}
			}
		}
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, [this, World]()
		{
			World->ServerTravel(TEXT("/Game/Levels/Dev_EmptyLobbyMap"), false, false);
		},
		5.f,
		false);
	}
}

void AFDGameMode::CallBossDead()
{
	if (not MovePlayersToLobbyTimerHandle.IsValid())
	{
		GetWorldTimerManager().SetTimer(MovePlayersToLobbyTimerHandle, [this]()
		{
			MovePlayersToLobby();
		},
		EndWaitTime,
		false);
	}
}

void AFDGameMode::FixupView(APlayerController* PlayerController)
{
	if (APawn* P = PlayerController->GetPawn())
	{
		PlayerController->bAutoManageActiveCameraTarget = false;
		PlayerController->SetViewTargetWithBlend(P, 0.f);
		if (auto* PS = PlayerController->GetPlayerState<AFDPlayerState>())
		{
			if (auto* Char = Cast<AFDPlayerCharacter>(P))
			{
				PS->PSASC->InitAbilityActorInfo(PS, Char);
				PS->BindDeathTagListener();
			}
		}
	}
}

void AFDGameMode::SpawnStartWall()
{
	FTransform SpawnTransform;
	// SpawnTransform.TransformVector(FVector(1320.f, 5970.f, 500.f));
	SpawnTransform.SetLocation(FVector(-1320.f, 5970.f, 500.f));
	SpawnTransform.SetRotation(FRotator(0.f, 0.f, 0.f).Quaternion());
	// SpawnTransform.SetScale3D(FVector(20.f, 20.f, 950.f));
	SpawnTransform.SetScale3D(FVector(1.f, 1.f, 1.f));
	
	// FVector SpawnLocation(1320.f, 5970.f, 500.f);
	// FRotator SpawnRotation(0.f, 0.f, 0.f);
	// FScale3D SpawnScale(0.f, 0.f, 0.f);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = nullptr;

	if (HasAuthority() && StartWallClass)
	{
		SpawnedStartWall = GetWorld()->SpawnActor<AActor>(
			StartWallClass,
			SpawnTransform,
			SpawnParams
		);

		if (!IsValid(SpawnedStartWall))
		{
			// UE_LOG(LogTemp, Error, TEXT("StartWall Not Spawned: %s"), *SpawnedStartWall->GetName());
			return;
		}

		SpawnedStartWall->SetActorScale3D(FVector(20.f, 20.f, 950.f));
	}
}

void AFDGameMode::SpawnBossExecutioner(int Level)
{
	FVector SpawnLocation(-830.f, -4620.f, 300.f);
	FRotator SpawnRotation(0.f, 90.f, 0.f);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = nullptr;

	if (HasAuthority() && BossExecutionerClass)
	{
		SpawnedBossExecutioner = GetWorld()->SpawnActor<AFDBossCharacter>(
			BossExecutionerClass,
			SpawnLocation,
			SpawnRotation,
			SpawnParams
		);

		if (!IsValid(SpawnedBossExecutioner))
		{
			// UE_LOG(LogTemp, Error, TEXT("BossExecutioner Not Spawned: %s"), *SpawnedBossExecutioner->GetName());
		}
	}
}

void AFDGameMode::SetDisableInput(APlayerController* InController)
{
	if (InController)
	{
		Cast<AFDPlayerController>(InController)->SetDisableInput();
	}
}

void AFDGameMode::GameOverAndMoveToLobby()
{
	UWorld* World = GetWorld();
	
	AActor* Boss = UGameplayStatics::GetActorOfClass(World, AFDBossCharacter::StaticClass());

	TArray<AActor*> MINs;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFDMINCharacter::StaticClass(), MINs);

	for (AActor* Actor : MINs)
	{
		if (AFDMINCharacter* MIN = Cast<AFDMINCharacter>(Actor))
		{
			if (AAIController* AICon = Cast<AAIController>(MIN->GetInstigatorController()))
			{
				AICon->StopMovement();
				AICon->BrainComponent->StopLogic(TEXT("AI Disabled"));
			}
			
			MIN->Destroy();
		}
	}
	
	if (Boss)
	{
		if (AAIController* AICon = Cast<AAIController>(Boss->GetInstigatorController()))
		{
			AICon->StopMovement();
			AICon->BrainComponent->StopLogic(TEXT("AI Disabled"));
		}

		FTimerHandle BossDestroyTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(BossDestroyTimerHandle, [Boss]()
		{
			Boss->Destroy();
		}, 7.f, false);
	}
	
	if (World)
	{
		for (TObjectPtr<APlayerState> PlayerStateIt : GetWorld()->GetGameState()->PlayerArray)
		{
			if (APlayerController* ControllerIt = PlayerStateIt->GetPlayerController())
			{
				if (AFDPlayerCharacter* Char = Cast<AFDPlayerCharacter>(ControllerIt->GetCharacter()))
				{
					AFDPlayerController* FDControllerIt = Cast<AFDPlayerController>(ControllerIt);
					FDControllerIt->Client_FailedAnim();
				}
			}
		}
	}

	if (not MovePlayersToLobbyTimerHandle.IsValid())
	{
		GetWorldTimerManager().SetTimer(MovePlayersToLobbyTimerHandle, [this]()
		{
			MovePlayersToLobby();
		},
		10.f,
		false);
	}
}



