// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/FDAnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFD_API UPlayerAnimInstance : public UFDAnimInstance
{
	GENERATED_BODY()
	
protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
	
private:
	bool bIsCombatTag;
	bool bIsDBNO;
	bool bIsJumping;
	bool bIsSprint;
	UPROPERTY()
	UAbilitySystemComponent* ASC;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Relax", meta = (AllowPrivateAccess = "true"))
	bool bShouldEnterRelaxState = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Relax", meta = (AllowPrivateAccess = "true"))
	float EnterRelaxStateThresold = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Relax", meta = (AllowPrivateAccess = "true"))
	float IdleElapsedTime = 0.f;
	
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	FORCEINLINE bool IsMoving() const { return Speed != 0; }
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	FORCEINLINE bool IsJumping() const { return bIsJumping; }
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	FORCEINLINE bool IsSprint() const { return bIsSprint; }
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	bool bCombatTag() const { return bIsCombatTag; }
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	bool bDBNOTag() const { return bIsDBNO; }
};
