// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_Trace.generated.h"


class ATA_Trace;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTraceResultDelegate, const FGameplayAbilityTargetDataHandle&,
                                            TargetDataHandle);
/**
 * 
 */
UCLASS()
class PROJECTFD_API UAT_Trace : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UAT_Trace();

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "WaitForTrace", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))

	static UAT_Trace* CreateTask(UGameplayAbility* OwningAbility, TSubclassOf<ATA_Trace> TargetActorClass);
	virtual void Activate() override;
	virtual void OnDestroy(bool AbilityEnded) override;

	void SpawnAndInitializeTargetActor();
	void FinalizeTargetActor();

protected:
	void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& DataHandle);
	
public:
	UPROPERTY(BlueprintAssignable)
	FTraceResultDelegate OnComplete;

protected:
	UPROPERTY()
	TSubclassOf<ATA_Trace> TargetActorClass;
	
	UPROPERTY()
	TObjectPtr<ATA_Trace> SpawnedTargetActor;
};
