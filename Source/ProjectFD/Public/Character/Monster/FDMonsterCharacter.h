// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Character/FDCharacter.h"
#include "Interfaces/PawnCombatInterface.h"
#include "FDMonsterCharacter.generated.h"

class AFDPlayerController;
class UDropItemDataAsset;
class UWidgetComponent;
struct FBossPart;
class UEnemyCombatComponent;
class UFDDataAsset_EnemyStartUpData;
class UEnemyAbilitySystemComponent;
class UEnemyAttributeSet;
class UMotionWarpingComponent;
/**
 * 
 */
UCLASS()
class PROJECTFD_API AFDMonsterCharacter : public AFDCharacter, public IPawnCombatInterface // Base
{
	GENERATED_BODY()

public:
	AFDMonsterCharacter();

	// 사용 안함
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY()
	bool bIsPunch;
	
protected:
	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UEnemyCombatComponent* EnemyCombatComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UEnemyAttributeSet* EnemyAttributeSet;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UFDDataAsset_EnemyStartUpData> EnemyStartUpData;

	UPROPERTY(EditAnywhere)
	UClass* DamageTextActorClass;
	
	// Base
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMotionWarpingComponent* MotionWarpingComponent;
	
private:
	void InitEnemyStartUpData();
	
public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE UAbilitySystemComponent* GetEnemyAbilitySystemComponent() const { return ASC; }

	FORCEINLINE UEnemyAttributeSet* GetEnemyAttributeSet() const { return EnemyAttributeSet; }
	
	FORCEINLINE UEnemyCombatComponent* GetEnemyCombatComponent() const { return EnemyCombatComponent; }

	UPROPERTY(ReplicatedUsing = OnRep_TargetActor, VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Target")
	AActor* TargetActor;
	
	UFUNCTION()
	void OnRep_TargetActor();

	UFUNCTION()
	void UpdateNearestTarget();

public:
	UFUNCTION()
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void HandleShieldAndHealth(float Damage, AActor* HitActor);

	UFUNCTION()
	bool ProcessBossPartDamage(AActor* HitActor, float Damage, FName HitBone);

	UFUNCTION()
	void SpawnWidget(float Damage, FVector SpawnLocation);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDropItemDataAsset* DropItemDataAsset;
	
	UFUNCTION()
	void DropItemOnDeath(AActor* HitActor);
	
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_ShowDamageUI(float Damage, FVector HitLocation, AController* InstigatorController);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_MonsterDead(AActor* HitActor);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateBossHUD(AActor* HitActor, float CurrentHp, float MaxHp, float CurrentShield, float MaxShield);
};
