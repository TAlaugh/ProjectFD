// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Projectile/FDProjectileBase.h"

#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "Compression/lz4.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AFDProjectileBase::AFDProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	ProjectileCollisionBox = CreateDefaultSubobject<UBoxComponent>("ProjectileCollisionBox");
	SetRootComponent(ProjectileCollisionBox);
	ProjectileCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	ProjectileCollisionBox->OnComponentHit.AddUniqueDynamic(this, &ThisClass::OnProjectileHit);
	ProjectileCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnProjectileBeginOverlap);
	ProjectileCollisionBox->IgnoreActorWhenMoving(GetOwner(), true);

	ProjectileNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("ProjectileNiagaraComponent");
	ProjectileNiagaraComponent->SetupAttachment(GetRootComponent());

	ProjectileParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>("ProjectileParticleComponent");
	ProjectileParticleComponent->SetupAttachment(GetRootComponent());

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	ProjectileMovementComponent->InitialSpeed = 800.f;
	ProjectileMovementComponent->MaxSpeed = 900.f;
	ProjectileMovementComponent->Velocity = FVector(1.f,0.f,0.f);
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;

	InitialLifeSpan = 4.f;

}

void AFDProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (ProjectileDamagePolicy == EProjectileDamagePolicy::OnBeginOverlap)
	{
		ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}

	// ProjectileCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (AActor* OwnerActor = GetOwner())
	{
		ProjectileCollisionBox->IgnoreActorWhenMoving(OwnerActor, true);

		ProjectileCollisionBox->MoveIgnoreActors.Add(OwnerActor);
	}

	// TWeakObjectPtr<AFDProjectileBase> WeakSelf(this);
	// FTimerHandle TimerHandle;
	// GetWorldTimerManager().SetTimer(TimerHandle, [WeakSelf]()
	// {
	// 	WeakSelf->ProjectileCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// }, 0.05f, false);
}

void AFDProjectileBase::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == GetOwner())
		return;
	
	if (Cast<AFDPlayerCharacter>(OtherActor))
	{
		UGameplayStatics::ApplyDamage(OtherActor, ProjectileDamage, GetOwner()->GetInstigatorController(), GetOwner(), nullptr);
	}

	Destroy();
}

void AFDProjectileBase::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetOwner())
		return;
	
	if (Cast<AFDPlayerCharacter>(OtherActor))
	{
		// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *OtherActor->GetActorNameOrLabel());
		UGameplayStatics::ApplyDamage(OtherActor, ProjectileDamage, GetOwner()->GetInstigatorController(), GetOwner(), nullptr);
	}

	Destroy();
}

