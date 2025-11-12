// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FDCharacter.h"

#include "Gameframework/FDPlayerState.h"
#include "GAS/FDAbilitySystemComponent.h"
#include "GAS/FDAttributeSet.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AFDCharacter::AFDCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
}

UAbilitySystemComponent* AFDCharacter::GetAbilitySystemComponent() const
{
	if (ASC)
	{
		return ASC;
	}

	return nullptr;
}

void AFDCharacter::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamID = NewTeamID;
}

FGenericTeamId AFDCharacter::GetGenericTeamId() const
{
	return TeamID;
}

void AFDCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFDCharacter, TeamID);
}

