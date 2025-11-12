// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/DropItem/DropItemBase.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "Components/BoxComponent.h"


// Sets default values
ADropItemBase::ADropItemBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SM = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(SM);
	SM->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	SM->SetCollisionResponseToAllChannels(ECR_Ignore);
	SM->SetSimulatePhysics(true);
	
	BoxCollision = CreateDefaultSubobject<UBoxComponent>("BoxCollision");
	BoxCollision->SetupAttachment(GetRootComponent());
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoxCollision->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::ItemOverlapEvent);

	NS = CreateDefaultSubobject<UNiagaraComponent>("NS");
	NS->SetupAttachment(GetRootComponent());
}

void ADropItemBase::BeginPlay()
{
	Super::BeginPlay();

	TWeakObjectPtr<ADropItemBase> WeakSelf(this);
	
	GetWorld()->GetTimerManager().SetTimer(DestroyItemTimer,
		[WeakSelf]()
		{
			if (WeakSelf.IsValid())
			{
				WeakSelf->Destroy();
			}
		}, 60.f, false);
}

void ADropItemBase::HandlePickUp(AFDPlayerCharacter* Player)
{
	// 이후 자식 구현
}

void ADropItemBase::ItemOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}
	
	AFDPlayerCharacter* Player = Cast<AFDPlayerCharacter>(OtherActor);
	if (Player)
	{
		// UE_LOG(LogTemp, Warning, TEXT("1"));
		HandlePickUp(Player);
		GetWorld()->GetTimerManager().ClearTimer(DestroyItemTimer);
		Destroy();
	}
}