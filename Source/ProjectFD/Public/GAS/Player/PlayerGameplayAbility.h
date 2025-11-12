// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/FDGameplayAbility.h"
#include "PlayerGameplayAbility.generated.h"

class UPlayerAbilitySystemComponent;
class UPlayerCombatComponent;
class AFDPlayerController;
class AFDPlayerCharacter;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UPlayerGameplayAbility : public UFDGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "GA")
	AFDPlayerCharacter* GetPlayerCharacterFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "GA")
	AFDPlayerController* GetPlayerControllerFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "GA")
	UPlayerCombatComponent* GetPlayerCombatComponentFromActorInfo();

	UPROPERTY(EditDefaultsOnly, Category = "Debug" )
	float DrawDebugDuration = 2.0f;
		
private:
	TWeakObjectPtr<AFDPlayerCharacter> CachedPlayerCharacter;
	TWeakObjectPtr<AFDPlayerController> CachedPlayerController;

	UPROPERTY(EditDefaultsOnly, Category = "Debug" )
	bool bShouldDrawDebug = true;


protected:
	UFUNCTION()
	FORCEINLINE bool ShouldDebugDraw() const { return bShouldDrawDebug; }
	
	TArray<FHitResult> GetHitResultFromSphereSweepLocationTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, float SphereSweetRadius = 30.0f, float TargetAmount = 0.0f, bool bDrawDebug = true, ETeamAttitude::Type TargetTeam = ETeamAttitude::Hostile, bool bIgnoreSelf = true) const;

	TArray<FHitResult> GetHitResultFromCapsuleSweepLocationTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, float CapsuleHalfHeight = 88.0f, float CapsuleRadius = 34.0f, float TargetAmount = 0.0f, bool bDrawDebug = true, ETeamAttitude::Type TargetTeam = ETeamAttitude::Hostile, bool bIgnoreSelf = true) const;
};
