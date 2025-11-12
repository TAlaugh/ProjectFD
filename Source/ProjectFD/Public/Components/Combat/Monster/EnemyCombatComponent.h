// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Combat/PawnCombatComponent.h"
#include "EnemyCombatComponent.generated.h"

class AFDMonsterCharacter;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UEnemyCombatComponent : public UPawnCombatComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	AFDMonsterCharacter* GetEnemyCharacter();

	UFUNCTION(BlueprintCallable)
	void ToggleCollision(bool bShouldEnable);
	
	// ~Base
	virtual void OnHitTargetActor(AActor* HitActor);
	virtual void OnCollisionPulledFromTargetActor(AActor* InteractedActor);

protected:
	virtual void BeginPlay() override;
	
protected:
	UPROPERTY()
	TArray<AActor*> OverlappedActor;
};
