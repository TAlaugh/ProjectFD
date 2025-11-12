// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibrary/FDFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GenericTeamAgentInterface.h"
#include "GAS/FDAbilitySystemComponent.h"
#include "Interfaces/PawnCombatInterface.h"


UFDAbilitySystemComponent* UFDFunctionLibrary::NativeGetASCFromActor(AActor* InActor)
{
	return CastChecked<UFDAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor));
	
}

void UFDFunctionLibrary::AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd)
{
	UFDAbilitySystemComponent* ASC = NativeGetASCFromActor(InActor);

	if (!ASC->HasMatchingGameplayTag(TagToAdd))
	{
		ASC->AddLooseGameplayTag(TagToAdd);
	}
}

void UFDFunctionLibrary::RemoveGameplayFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove)
{
	UFDAbilitySystemComponent* ASC = NativeGetASCFromActor(InActor);

	if (ASC->HasMatchingGameplayTag(TagToRemove))
	{
		ASC->RemoveLooseGameplayTag(TagToRemove);
	}
}

bool UFDFunctionLibrary::NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck)
{
	UFDAbilitySystemComponent* ASC = NativeGetASCFromActor(InActor);

	return ASC->HasMatchingGameplayTag(TagToCheck);
}

void UFDFunctionLibrary::BP_NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck,
	EFDConfirmType& OutConfirmType)
{
	OutConfirmType = NativeDoesActorHaveTag(InActor, TagToCheck) ? EFDConfirmType::Yes : EFDConfirmType::No;
}

UPawnCombatComponent* UFDFunctionLibrary::NativeGetPawnCombatComponentFromActor(AActor* InActor)
{
	check(InActor);
	
	if (IPawnCombatInterface* PawnCombatInterface = Cast<IPawnCombatInterface>(InActor))
	{
		return PawnCombatInterface->GetPawnCombatComponent();
	}

	return nullptr;
}

UPawnCombatComponent* UFDFunctionLibrary::BP_GetPawnCombatComponentFromActor(AActor* InActor,
	EFDValidType& OutValidType)
{
	UPawnCombatComponent* CombatComponent = NativeGetPawnCombatComponentFromActor(InActor);

	OutValidType = CombatComponent ? EFDValidType::Valid : EFDValidType::InValid;

	return CombatComponent;
}

bool UFDFunctionLibrary::IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn)
{
	if (QueryPawn && TargetPawn)
	{
		IGenericTeamAgentInterface* QueryTeamAgent = Cast<IGenericTeamAgentInterface>(QueryPawn->GetController());
		IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(TargetPawn->GetPlayerState());

		if (QueryTeamAgent && TargetTeamAgent)
		{
			return QueryTeamAgent->GetGenericTeamId() != TargetTeamAgent->GetGenericTeamId();
		}
	}
	return false;
}
