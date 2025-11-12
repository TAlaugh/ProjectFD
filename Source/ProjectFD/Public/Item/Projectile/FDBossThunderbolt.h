// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FDBossThunderbolt.generated.h"

class UNiagaraSystem;

UCLASS()
class PROJECTFD_API AFDBossThunderbolt : public AActor
{
	GENERATED_BODY()
	
public:	
	AFDBossThunderbolt();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void SpawnThunder();

	UFUNCTION()
	void SpawnWarningEffect();

	UFUNCTION()
	void SpawnThunderEffect();

public:
	UPROPERTY(EditAnywhere, Category = "Thunderbolt")
	float Damage = 0.f;

	UPROPERTY(EditAnywhere, Category = "Thunderbolt")
	float ThunderboltRadius = 0.f;
	
	UPROPERTY(EditAnywhere, Category = "Thunderbolt")
	UNiagaraSystem* WarningEffect;
	
	UPROPERTY(EditAnywhere, Category = "Thunderbolt")
	UNiagaraSystem* ThunderEffect;

	UPROPERTY(EditAnywhere, Category = "Thunderbolt")
	USoundBase* ThunderSound;
};
