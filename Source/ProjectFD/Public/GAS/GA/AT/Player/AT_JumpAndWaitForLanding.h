// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_JumpAndWaitForLanding.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FJumpAndWaitForLandingDelegate);
/**
 * 
 */
UCLASS()
class PROJECTFD_API UAT_JumpAndWaitForLanding : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UAT_JumpAndWaitForLanding();

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "JumpAndWaitForLanding", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAT_JumpAndWaitForLanding* CreateTask(UGameplayAbility* OwningAbility);

	virtual void Activate() override;
	virtual void OnDestroy(bool AbilityEnded) override;

	UPROPERTY(BlueprintAssignable)
	FJumpAndWaitForLandingDelegate OnComplete;

protected:
	UFUNCTION()
	void OnLandedCallback(const FHitResult& Hit);
};
