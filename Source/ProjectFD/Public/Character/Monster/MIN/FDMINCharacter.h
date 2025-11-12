// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Monster/FDMonsterCharacter.h"
#include "FDMINCharacter.generated.h"

/**
 * 
 */

class UBoxComponent;

DECLARE_DELEGATE_OneParam(FOnTargetInteractedDelegate, AActor*)

UCLASS()
class PROJECTFD_API AFDMINCharacter : public AFDMonsterCharacter
{
	GENERATED_BODY()

public:
	AFDMINCharacter();
	
	// Delegate
	FOnTargetInteractedDelegate OnCollisionHitTarget;
	FOnTargetInteractedDelegate OnCollisionPulledFromTarget;

	// Getter
	UBoxComponent* GetMeleeCollision() const { return BoxCollision; }

	UFUNCTION()
	void DropMonsterItem();
	
protected:
	virtual void BeginPlay() override;
	
	// Collision
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Collision")
	UBoxComponent* BoxCollision;
	
	UFUNCTION()
	virtual void OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	
};
