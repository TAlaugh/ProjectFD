// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FDGameState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFD_API AFDGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AFDGameState();

	// 공유 부활 횟수 (전원 공용)
	UPROPERTY(ReplicatedUsing=OnRep_SharedRespawns, VisibleAnywhere, BlueprintReadOnly, Category="Respawn")
	int32 SharedRespawns = 0;

	UFUNCTION(BlueprintCallable, Category="Respawn")
	bool CanRespawnShared() const;

	// 서버에서만 호출
	UFUNCTION(BlueprintCallable, Category="Respawn")
	void SetSharedRespawns(int32 Count);

	// 서버에서만 호출: 공유 부활 1 소모
	UFUNCTION(BlueprintCallable, Category="Respawn")
	void ConsumeSharedRespawn();

protected:
	UFUNCTION()
	void OnRep_SharedRespawns();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};