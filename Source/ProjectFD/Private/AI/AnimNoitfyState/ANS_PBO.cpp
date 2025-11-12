// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AnimNoitfyState/ANS_PBO.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "Item/Projectile/FDBossNormalLaser.h"
#include "Kismet/GameplayStatics.h"

void UANS_PBO::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (!MeshComp)
	{
		return;
	}
	
	PrevEnd = MeshComp->GetSocketLocation(MuzzleSocketName);

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor || !OwnerActor->HasAuthority())
	{
		return;
	}

	AAIController* AIC = Cast<AAIController>(OwnerActor->GetInstigatorController());
	if (!AIC) return;
	UBlackboardComponent* BB = AIC->GetBlackboardComponent();
	if (!BB) return;
	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject("TargetActor"));
	if (!TargetActor) return;

	TargetLocation = TargetActor->GetActorLocation();

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	LaserEffectActor = MeshComp->GetWorld()->SpawnActor<AFDBossNormalLaser>(
					LaserEffectActorClass,
					MeshComp->GetSocketLocation(MuzzleSocketName),
					FRotator::ZeroRotator,
					SpawnParameters
				);
}

void UANS_PBO::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	if (!MeshComp)
	{
		return;
	}

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor || !OwnerActor->HasAuthority())
	{
		return;
	}

	AAIController* AIC = Cast<AAIController>(OwnerActor->GetInstigatorController());
	if (!AIC) return;
	UBlackboardComponent* BB = AIC->GetBlackboardComponent();
	if (!BB) return;
	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject("TargetActor"));
	if (!TargetActor) return;
	
	FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
	
	TimerAccumulator += FrameDeltaTime;

	FVector Dir = (TargetLocation - MuzzleLocation).GetSafeNormal();
	float Distance = (TargetLocation - MuzzleLocation).Size();
	float LaserLength = Distance + ExtraLength;
	FVector End = MuzzleLocation + Dir * LaserLength;
	
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(MeshComp->GetOwner());

	bool bHit = MeshComp->GetWorld()->SweepSingleByChannel(HitResult,MuzzleLocation, End,FQuat::Identity,
		ECC_Visibility, FCollisionShape::MakeSphere(30.f), Params);

	if (LaserEffectActor)
	{
		LaserEffectActor->SetActorLocation(MuzzleLocation);

		FRotator NewRot = Dir.Rotation() + FRotator(180.f, 0.f, 0.f);
		LaserEffectActor->SetActorRotation(NewRot);
		
		LaserEffectActor->NetMulticast_PBOUpdateTargetLocation(End, MuzzleLocation);
	}
	
	if (TimerAccumulator >= TraceInterval)
	{
		TimerAccumulator = 0.f;

		if (bHit)
		{
			if (Cast<AFDPlayerCharacter>(HitResult.GetActor()))
			{
				// GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, *HitResult.GetActor()->GetActorNameOrLabel());
				// 데미지 적용
				UGameplayStatics::ApplyDamage(HitResult.GetActor(), Damage, OwnerActor->GetInstigatorController(), OwnerActor, nullptr);
				
			}
		}
	
		// DrawDebugLine(MeshComp->GetWorld(), MuzzleLocation, End, FColor::Red, false, 0.1f, 0, 30.f);
	}
}

void UANS_PBO::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (LaserEffectActor)
	{
		LaserEffectActor->Destroy();
	}
}
