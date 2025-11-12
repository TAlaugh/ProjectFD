// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Monster/MIN/FDMINCharacter.h"

#include "Components/BoxComponent.h"
#include "DataAssets/DropItem/DropItemDataAsset.h"
#include "FunctionLibrary/FDFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"


AFDMINCharacter::AFDMINCharacter()
{
	BoxCollision = CreateDefaultSubobject<UBoxComponent>("Melee_Collision");
	BoxCollision->SetupAttachment(GetMesh(), "Bn_MON_MIN_1002_5");

	BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AFDMINCharacter::DropMonsterItem()
{
	if (!HasAuthority()) return;
	
	if (!DropItemDataAsset) return;

	int32 DropCount = FMath::RandRange(0, 2);
	for (int32 i = 0; i < DropCount; i++)
	{
		const TArray<FDropItemData>& Items = DropItemDataAsset->MonsterDrops;
		if (Items.Num() == 0) continue;

		float TotalWeight = 0.f;
		for (const FDropItemData& Item : Items)
		{
			TotalWeight += Item.DropRate;
		}

		float RandomPoint = FMath::FRandRange(0.f, TotalWeight);
		float Accum = 0.f;

		for (const FDropItemData& Item : Items)
		{
			Accum += Item.DropRate;
			if (RandomPoint <= Accum)
			{
				if (Item.DropItemClass)
				{
					FVector SpawnLoc = GetActorLocation() + FVector(FMath::RandRange(-100,100), FMath::RandRange(-100,100), 50);
					GetWorld()->SpawnActor<ADropItemBase>(Item.DropItemClass, SpawnLoc, FRotator::ZeroRotator);
				}
				break;
			}
		}
	}
}

void AFDMINCharacter::BeginPlay()
{
	Super::BeginPlay();

	BoxCollision->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionBeginOverlap);
	BoxCollision->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionEndOverlap);
}

void AFDMINCharacter::OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* OwningPawn = GetInstigator<APawn>();
	
	if (!OwningPawn)
	{
		return;
	}
	
	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (OwningPawn != HitPawn)
		{
			OnCollisionHitTarget.ExecuteIfBound(OtherActor);
			if (UFDFunctionLibrary::IsTargetPawnHostile(OwningPawn, HitPawn))
			{
				// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *OtherActor->GetActorNameOrLabel());
				UGameplayStatics::ApplyDamage(HitPawn, 10.f, GetOwner()->GetInstigatorController(), GetOwner(), nullptr);
				
				FVector KnockbackDir = (HitPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();
				FVector LaunchForce = KnockbackDir * 500.f + FVector(0, 0, 250.f);
				
				ACharacter* HitCharacter = Cast<ACharacter>(HitPawn);
				HitCharacter->LaunchCharacter(LaunchForce, true, true);
			}
		}
	}
}

void AFDMINCharacter::OnCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APawn* OwningPawn = GetInstigator<APawn>();

	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (OwningPawn != HitPawn)
		{
			OnCollisionPulledFromTarget.ExecuteIfBound(OtherActor);
		}
	}
}
