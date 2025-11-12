// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/LineTraceActor.h"
#include "TimerManager.h"


// Sets default values
ALineTraceActor::ALineTraceActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALineTraceActor::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle TraceTimerHandle;

	GetWorldTimerManager().SetTimer(TraceTimerHandle, this, &ALineTraceActor::DrawLineTrace, 1.0f, true);
	
}

// Called every frame
void ALineTraceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALineTraceActor::DrawLineTrace()
{
	FCollisionQueryParams Params;
	Params.bTraceComplex = true;
	Params.bReturnPhysicalMaterial = true;
	Params.AddIgnoredActor(this);
	Params.bTraceIntoSubComponents = true;
	

	FHitResult OutHit;

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		OutHit,
		GetActorLocation(),
		GetActorLocation() + FVector(1000, 0, 0),
		ECC_Visibility,
		Params
	);

	UE_LOG(LogTemp, Warning, TEXT("%s"), *OutHit.GetComponent()->GetReadableName());

	FColor LineColor = bHit ? FColor::Red : FColor::Green; // 맞으면 빨강, 아니면 초록
	DrawDebugLine(
		GetWorld(),
		GetActorLocation(),
		GetActorLocation() + FVector(1000, 0, 0),
		LineColor,
		true,
		2.0f,
		0,
		2.0f
	);
}

