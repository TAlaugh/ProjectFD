// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EquipmentActor.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FWeaponFireSignature, bool)
DECLARE_MULTICAST_DELEGATE_OneParam(FWeaponReloadSignature, bool)

UCLASS()
class PROJECTFD_API AEquipmentActor : public AActor
{
	GENERATED_BODY()
	
public:
	
	AEquipmentActor();

	virtual void SetOwner(AActor* NewOwner) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// UPROPERTY(EditDefaultsOnly)
	// bool bIsActiveSlot = false;

	UFUNCTION()
	bool GetIsEquipped();
	UFUNCTION()
	void SetIsEquipped(bool InIsEquipped);

	UFUNCTION()
	void PlayFireMontage(bool InShouldFire);
	UFUNCTION()
	void PlayReloadMontage(bool InIsReloading);

	float GetFireMontageLength();

	// UFUNCTION()
	// bool GetShouldFire();
	// UFUNCTION()
	// void SetShouldFire(bool bShouldFire);
	//
	// UFUNCTION()
	// bool GetIsReloading();
	// UFUNCTION()
	// void SetIsReloading(bool bIsReloading);

	void SetWeaponVisibility(bool InIsVisible);
	
	FWeaponFireSignature WeaponFireDelegate;
	FWeaponReloadSignature WeaponReloadDelegate;

	UFUNCTION(BlueprintPure, Category="Animation")
	UAnimMontage* GetReloadMontage() const { return ReloadMontage; }

private:

	UFUNCTION(Server, Unreliable)
	void ServerSetVisibility(bool InIsVisible);

	UFUNCTION()
	void OnRep_SetVisibility();

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<USceneComponent> RootScene;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> EquipmentMesh;

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	TObjectPtr<UAnimMontage> FireMontage;

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	TObjectPtr<UAnimMontage> ReloadMontage;

	UPROPERTY(ReplicatedUsing=OnRep_SetVisibility)
	bool bIsWeaponVisible = true;

	UPROPERTY(Replicated)
	bool bIsEquipped = false;
	UPROPERTY()
	bool bShouldFire = false;
	UPROPERTY()
	bool bIsReloading = false;
	
};
