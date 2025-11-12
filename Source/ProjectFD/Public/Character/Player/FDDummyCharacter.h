// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/FDCharacter.h"
#include "FDDummyCharacter.generated.h"

class UGameplayEffect;
class UPlayerAttributeSet;
/**
 * 
 */
UCLASS()
class PROJECTFD_API AFDDummyCharacter : public AFDCharacter
{
	GENERATED_BODY()

public:
	AFDDummyCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void PossessedBy(AController* NewController) override;

protected:

	UPROPERTY(EditAnywhere, Category = "GAS")
	TObjectPtr<UFDAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, Category = "GAS")
	TArray<TSubclassOf<UGameplayEffect>> InitStatEffect;

	UPROPERTY(EditAnywhere, Category = "GAS")
	float Level;
};
