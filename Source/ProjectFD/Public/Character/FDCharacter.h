// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Character.h"
#include "FDCharacter.generated.h"

class UFDAbilitySystemComponent;
class UAbilitySystemComponent;
class UFDAttributeSet;

UCLASS()
class PROJECTFD_API AFDCharacter : public ACharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFDCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAbilitySystemComponent* ASC;

	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	
	UPROPERTY(Replicated)
	FGenericTeamId TeamID;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
};
