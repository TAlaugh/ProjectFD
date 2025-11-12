// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Controller/FDPlayerController.h"
#include "TESTPLAYERCONTROLLER.generated.h"

class UGameplayEffect;
/**
 * 
 */
UCLASS()
class PROJECTFD_API ATESTPLAYERCONTROLLER : public AFDPlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
public:
	ATESTPLAYERCONTROLLER();
	
	virtual FGenericTeamId GetGenericTeamId() const override;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	UPROPERTY(Replicated)
	FGenericTeamId PlayerTeamID;
};
