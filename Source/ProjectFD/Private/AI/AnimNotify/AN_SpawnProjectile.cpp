// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AnimNotify/AN_SpawnProjectile.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Item/Projectile/FDProjectileBase.h"


void UAN_SpawnProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (!MeshComp || !ProjectileClass) return;

	ACharacter* OwnerChar = Cast<ACharacter>(MeshComp->GetOwner());
	if (!OwnerChar) return;

	if (!OwnerChar->HasAuthority())
	{
		return;
	}

	AAIController* AIC =  Cast<AAIController>(OwnerChar->GetInstigatorController());
	UBlackboardComponent* BB = AIC->GetBlackboardComponent();
	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject("TargetActor"));
	if (!AIC || !BB || !TargetActor)
	{
		return;
	}
	
	FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName); 
	
	FVector TargetLocation = TargetActor->GetActorLocation();
	
	FVector Dir = (TargetLocation - MuzzleLocation).GetSafeNormal();
	
	// 소켓 위치 가져오기
	FVector SpawnLocation = MuzzleLocation;

	FRotator SpawnRotation = Dir.Rotation();

	// 스폰 파라미터
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerChar;
	SpawnParams.Instigator = OwnerChar;

	// Projectile 스폰
	AFDProjectileBase* Projectile = GetWorld()->SpawnActor<AFDProjectileBase>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
}
