// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/TESTPLAYERCONTROLLER.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "Net/UnrealNetwork.h"


ATESTPLAYERCONTROLLER::ATESTPLAYERCONTROLLER()
{
	PlayerTeamID = FGenericTeamId(0);
}

FGenericTeamId ATESTPLAYERCONTROLLER::GetGenericTeamId() const
{
	return PlayerTeamID;
}

void ATESTPLAYERCONTROLLER::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATESTPLAYERCONTROLLER, PlayerTeamID);
}
