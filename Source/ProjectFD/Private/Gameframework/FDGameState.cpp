// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameframework/FDGameState.h"

#include "Controller/FDPlayerController.h"
#include "Gameframework/FDGameMode.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/SpectatorPawn.h"
#include "Net/UnrealNetwork.h"
#include "Widget/HUD/BossStatusHUD.h"


AFDGameState::AFDGameState()
{
}

bool AFDGameState::CanRespawnShared() const
{
	return SharedRespawns > 0;
}

void AFDGameState::SetSharedRespawns(int32 Count)
{
	if (HasAuthority())
	{
		SharedRespawns = FMath::Max(0, Count);
		OnRep_SharedRespawns();
	}
}

void AFDGameState::ConsumeSharedRespawn()
{
	if (!HasAuthority()) return;
	
	if (SharedRespawns > 1)
	{
		SharedRespawns -= 1;
		OnRep_SharedRespawns();
	}
	else
	{
		AFDGameMode* GM = GetWorld()->GetAuthGameMode<AFDGameMode>();
		if (GM)
		{
			UE_LOG(LogTemp, Warning, TEXT("남은 부활 없음 -> 10초 후 게임 종료"));
			GM->GameOverAndMoveToLobby();
		}
	}
}

void AFDGameState::OnRep_SharedRespawns()
{
	// 여기서 UI 업데이트 등 필요한 작업 수행
	UE_LOG(LogTemp, Log, TEXT("남은 목숨 : %d"), SharedRespawns - 1);

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->IsLocalController())
		{
			if (UBossStatusHUD* BossHUD = Cast<AFDPlayerController>(PC)->BossStatusHUD)
			{
				BossHUD->SetDeathCountText(SharedRespawns - 1);
			}
			break;
		}
	}
}

void AFDGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFDGameState, SharedRespawns);
}
