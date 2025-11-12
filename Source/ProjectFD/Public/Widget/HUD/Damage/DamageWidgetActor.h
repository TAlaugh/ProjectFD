// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageWidgetActor.generated.h"

class UWidgetComponent;

UCLASS()
class PROJECTFD_API ADamageWidgetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADamageWidgetActor();

	void InitDamage(float Damage);
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UWidgetComponent* WidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UUserWidget> WidgetClass;
	
};
