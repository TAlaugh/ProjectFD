// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameframework/FDPlayerState.h"

#include "Character/Player/FDPlayerCharacter.h"
#include "Gameframework/FDGameState.h"
#include "GAS/GE/Player/GEPlayerFullStat.h"
#include "GAS/Player/PlayerAbilitySystemComponent.h"
#include "GAS/FDAbilitySystemComponent.h"
#include "GAS/FDGameplayStatic.h"
#include "GAS/Player/PlayerAttributeSet.h"
#include "Inventory/InventoryComponent.h"
#include "Net/UnrealNetwork.h"


AFDPlayerState::AFDPlayerState()
{
	// AbilitySystemComponent 생성 및 설정
	PSASC = CreateDefaultSubobject<UPlayerAbilitySystemComponent>(TEXT("PlayerAbilitySystemComponent"));
	PSASC->SetIsReplicated(true);
	PSASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// AttributeSet 생성
	PlayerAttributeSet = CreateDefaultSubobject<UPlayerAttributeSet>(TEXT("PlayerAttributeSet"));

	NetUpdateFrequency = 100.0f;

	// TeamID 설정
	TeamID = FGenericTeamId(0);

	if (!DeathTag.IsValid())
	{
		DeathTag = FGameplayTag::RequestGameplayTag(FName("Player.Status.Dead"), false);
	}
}

void AFDPlayerState::SeamlessTravelTo(class APlayerState* NewPlayerState)
{
	Super::SeamlessTravelTo(NewPlayerState);
	
	if (AFDPlayerState* PS = Cast<AFDPlayerState>(NewPlayerState))
	{
		PS->PlayerNum = PlayerNum;
		PS->CachedInventoryList = CachedInventoryList;
		PS->CachedEquipmentList = CachedEquipmentList;
	}
}

void AFDPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
	
	if (AFDPlayerState* PS = Cast<AFDPlayerState>(PlayerState))
	{
		PS->CachedInventoryList = CachedInventoryList;
		PS->CachedEquipmentList = CachedEquipmentList;
	}
}

UAbilitySystemComponent* AFDPlayerState::GetAbilitySystemComponent() const
{
	return PSASC;
}

void AFDPlayerState::CaptureInventories(AFDPlayerCharacter* InPlayer)
{
	if (HasAuthority())
	{
		CachedInventoryList = InPlayer->GetInventoryComponent_Implementation()->GetInventoryEntries();
		CachedEquipmentList = InPlayer->GetEquipmentComponent()->EquipmentList.GetWeaponEntries();
	}
}

void AFDPlayerState::RestoreInventories(AFDPlayerCharacter* InPlayer)
{
	if (HasAuthority())
	{
		if (not CachedInventoryList.IsEmpty())
		{
			InPlayer->GetInventoryComponent_Implementation()->InventoryList.SetInventoryEntries(CachedInventoryList);
		}
		if (not CachedEquipmentList.IsEmpty())
		{
			InPlayer->GetEquipmentComponent()->EquipmentList.SetWeaponEntries(CachedEquipmentList);
		}
	}
}

bool AFDPlayerState::GetIsLobby()
{
	return bIsLobby;
}

void AFDPlayerState::SetIsLobby(bool InIsLobby)
{
	bIsLobby = InIsLobby;
}

void AFDPlayerState::BeginPlay()
{
	Super::BeginPlay();

}

void AFDPlayerState::AddStartupAbilities()
{
	if (HasAuthority())
	{
		// 여기에 기본 능력을 추가하는 로직을 구현합니다.
		//GetAbilitySystemComponent()->GiveAbility(FGameplayAbilitySpec(UGEPlayerFullStat::StaticClass(), 1, 0));
		//GetAbilitySystemComponent()->InitAbilityActorInfo(this, GetPawn());
		PSASC->ApplyInitializeEffects();
		PSASC->OperatingInitializedAbilities();
	}
}

FGenericTeamId AFDPlayerState::GetGenericTeamId() const
{
	return TeamID;
}

void AFDPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFDPlayerState, PSASC);
	DOREPLIFETIME(AFDPlayerState, TeamID);
	DOREPLIFETIME(AFDPlayerState, bIsLobby);
}

void AFDPlayerState::BindDeathTagListener()
{
	if (bBoundDeathTag || !PSASC || !DeathTag.IsValid())
	{
		return;
	}

	// Death 태그 부여/제거 이벤트 구독
	PSASC->RegisterGameplayTagEvent(DeathTag, EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &AFDPlayerState::OnDeathTagChanged);

	bBoundDeathTag = true;
}

void AFDPlayerState::OnDeathTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (!HasAuthority())
	{
		return;
	}

	// 태그가 0->1로 처음 붙을 때만 공유 부활 1회 차감
	if (NewCount > 0)
	{
		if (!bCountedThisDeath)
		{
			bCountedThisDeath = true;

			if (UWorld* World = GetWorld())
			{
				if (AFDGameState* GS = World->GetGameState<AFDGameState>())
				{
					GS->ConsumeSharedRespawn();
				}
			}
		}
	}
	else
	{
		// 태그 제거되면 다음 사망을 위해 리셋
		bCountedThisDeath = false;
	}
}
