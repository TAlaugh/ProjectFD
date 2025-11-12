// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "FDAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;

/**
 * 
 */
UCLASS()
class PROJECTFD_API AFDAIController : public AAIController
{
	GENERATED_BODY()

public:
	AFDAIController();

	// ~ Begin IGenericTeamAgentInterface
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	// ~ End IGenericTeamAgentInterface

	virtual void OnPossess(APawn* InPawn) override;

	virtual void BeginPlay() override;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "AI Behavior")
	FName TargetActorBlackboardKeyName = "TargetActor";	
	
	UPROPERTY(EditDefaultsOnly, Category = "AI Behavior")
	UBehaviorTree* BehaviorTree;
	
	// ~ Begin Enemy Perception
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perception")
	UAIPerceptionComponent* EnemyPerceptionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perception")
	UAISenseConfig_Sight* AISenseConfig_Sight;

	UFUNCTION()
	virtual void OnEnemyPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	// ~ End Enemy Perception

	const UObject* GetCurrentTarget() const;
	void SetCurrentTarget(AActor* NewTarget);

	UPROPERTY(ReplicatedUsing = OnRep_TargetActor)
	AActor* TargetActor;

	UFUNCTION()
	void OnRep_TargetActor();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
