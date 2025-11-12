// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FDFunctionLibrary.generated.h"

class UPawnCombatComponent;
class UFDAbilitySystemComponent;

UENUM(BlueprintType)
enum class EFDConfirmType : uint8
{
	Yes,
	No
};

UENUM(BlueprintType)
enum class EFDValidType : uint8
{
	Valid,
	InValid
};

/**
 * 
 */
UCLASS()
class PROJECTFD_API UFDFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static UFDAbilitySystemComponent* NativeGetASCFromActor(AActor* InActor);

	UFUNCTION(BlueprintCallable, Category = "FD|FunctionLibrary")
	static void AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd);
	
	UFUNCTION(BlueprintCallable, Category = "FD|FunctionLibrary")
	static void RemoveGameplayFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove);

	static bool NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck);

	UFUNCTION(BlueprintCallable, Category = "FD|FunctionLibrary", meta = (DisplayName = "Does Actor Have Tag", ExpandEnumAsExecs = "OutConfirmType"))
	static void BP_NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck, EFDConfirmType& OutConfirmType);

	static UPawnCombatComponent* NativeGetPawnCombatComponentFromActor(AActor* InActor);

	UFUNCTION(BlueprintCallable, Category = "FD|FunctionLibrary", meta = (DisplayName = "Get Pawn Combat Component From Actor", ExpandEnumAsExecs = "OutValidType"))
	static UPawnCombatComponent* BP_GetPawnCombatComponentFromActor(AActor* InActor, EFDValidType& OutValidType);

	UFUNCTION(BlueprintPure, Category = "FD|FunctionLibrary")
	static bool IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn);
	
};
