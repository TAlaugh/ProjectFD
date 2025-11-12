// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DropItemBase.generated.h"

class AFDPlayerCharacter;
class UNiagaraComponent;
class UBoxComponent;

UCLASS()
class PROJECTFD_API ADropItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ADropItemBase();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* SM;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* BoxCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraComponent* NS;

	UPROPERTY()
	FTimerHandle DestroyItemTimer;
	
	UFUNCTION()
	virtual void HandlePickUp(AFDPlayerCharacter* Player);
	
	UFUNCTION()
	virtual void ItemOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
};
