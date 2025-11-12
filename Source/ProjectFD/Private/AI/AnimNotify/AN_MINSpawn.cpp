// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AnimNotify/AN_MINSpawn.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"


void UAN_MINSpawn::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp || !MinionClass) return;
	
	AActor* OwnerActor = MeshComp->GetOwner();
	
	if (!OwnerActor || !OwnerActor->HasAuthority()) return;
	
	UWorld* World = OwnerActor->GetWorld();
	if (!World) return;
	
	AAIController* BossAI = Cast<AAIController>(OwnerActor->GetInstigatorController());
	if (!BossAI) return;
	
	UBlackboardComponent* BB = BossAI->GetBlackboardComponent();
	if (!BB) return;
	
	AActor* Target = Cast<AActor>(BB->GetValueAsObject("TargetActor"));
	if (!Target) return;
	
	FVector TargetLocation = Target->GetActorLocation();
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	if (!NavSys) return;
	
	for (int32 i = 0; i < SpawnCount; ++i)
	{
		float RandRadius = FMath::FRandRange(SpawnRadius - 150.f, SpawnRadius);
	
		FNavLocation NavLocation;
		bool bFound = NavSys->GetRandomReachablePointInRadius(TargetLocation, RandRadius, NavLocation);
	
		if (!bFound)
		{
			continue;
		}
	
		FVector SpawnPos = NavLocation.Location;
		FRotator SpawnRot = (TargetLocation - SpawnPos).Rotation();
	
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
		World->SpawnActor<AActor>(MinionClass, SpawnPos, SpawnRot, SpawnParams);
	}
}
