// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/FDAIController.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "GenericTeamAgentInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/FDCharacter.h"
#include "Microsoft/COMPointer.h"
#include "Net/UnrealNetwork.h"

AFDAIController::AFDAIController()
{
	// bReplicates = true;

	TargetActor = nullptr;
	
	EnemyPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("EnemyPerceptionComponent");
	AISenseConfig_Sight = CreateDefaultSubobject<UAISenseConfig_Sight>("EnemySenseConfig_Sight");
	
	AISenseConfig_Sight->DetectionByAffiliation.bDetectEnemies = true;
	AISenseConfig_Sight->DetectionByAffiliation.bDetectFriendlies = false;
	AISenseConfig_Sight->DetectionByAffiliation.bDetectNeutrals = false;
	AISenseConfig_Sight->SightRadius = 5000.f;
	AISenseConfig_Sight->LoseSightRadius = 5200.f;
	AISenseConfig_Sight->PeripheralVisionAngleDegrees = 180.f;

	EnemyPerceptionComponent->ConfigureSense(*AISenseConfig_Sight);
	EnemyPerceptionComponent->SetDominantSense(UAISenseConfig_Sight::StaticClass());
	EnemyPerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &AFDAIController::OnEnemyPerceptionUpdated);
}

ETeamAttitude::Type AFDAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	const APawn* PawnToCheck = Cast<const APawn>(&Other);

	// const IGenericTeamAgentInterface* OtherTeamAgent = Cast<const IGenericTeamAgentInterface>(PawnToCheck->GetController());
	const IGenericTeamAgentInterface* OtherTeamAgent = Cast<const IGenericTeamAgentInterface>(PawnToCheck->GetPlayerState());

	if (OtherTeamAgent && OtherTeamAgent->GetGenericTeamId() < GetGenericTeamId())
	{
		return ETeamAttitude::Hostile;
	}

	return ETeamAttitude::Friendly;
}

void AFDAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	SetGenericTeamId(FGenericTeamId(1));

	IGenericTeamAgentInterface* PawnTeamInterface = Cast<IGenericTeamAgentInterface>(InPawn);
	if (PawnTeamInterface)
	{
		PawnTeamInterface->SetGenericTeamId(GetGenericTeamId());
	}
}

void AFDAIController::BeginPlay()
{
	Super::BeginPlay();
	// RunBehaviorTree(BehaviorTree);
}

void AFDAIController::OnEnemyPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed() && Actor)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, Actor->GetActorNameOrLabel() + (" was Sensed"));
		if (!GetCurrentTarget())
		{
			SetCurrentTarget(Actor);
		}
	}
	else
	{
		if (GetCurrentTarget() == Actor)
		{
			SetCurrentTarget(nullptr);
		}
	}
}

const UObject* AFDAIController::GetCurrentTarget() const
{
	const UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
	if (BlackboardComponent)
	{
		return GetBlackboardComponent()->GetValueAsObject(TargetActorBlackboardKeyName);
	}
	return nullptr;
}

void AFDAIController::SetCurrentTarget(AActor* NewTarget)
{
	UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		return;
	}
	
	if (NewTarget)
	{
		BlackboardComponent->SetValueAsObject(TargetActorBlackboardKeyName, NewTarget);
		TargetActor = NewTarget;
		OnRep_TargetActor();
	}
	else
	{
		BlackboardComponent->ClearValue(TargetActorBlackboardKeyName);
	}
}

void AFDAIController::OnRep_TargetActor()
{
	
}

void AFDAIController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFDAIController, TargetActor);
}
