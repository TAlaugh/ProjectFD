// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Player/PlayerGameplayAbility.h"
#include "WireAction2Ability.generated.h"

class UNiagaraSystem;
class UFDGameplayStatic;
class ACableActor;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UWireAction2Ability : public UPlayerGameplayAbility
{
	GENERATED_BODY()

public:
	UWireAction2Ability();
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
private:
	FTimerHandle EndTimerHandle;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	UAnimMontage* FireMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	UAnimMontage* LoopMontage = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<UGameplayEffect> CooldownGEClass;

	UPROPERTY(EditDefaultsOnly, Category="Skill")
	FGameplayTag CooldownTag;

	UPROPERTY(EditDefaultsOnly, Category="Skill")
	FName MuzzleSocketName = TEXT("Bn_Socket_L_Hand");

	UPROPERTY(EditDefaultsOnly, Category="Skill")
	float SkillCooldown = 10.f;

	UPROPERTY(EditDefaultsOnly, Category="Skill")
	float SkillDuration = 3.f;

	UPROPERTY(EditDefaultsOnly, Category="Skill")
	USoundBase* FireSound;
	
	UPROPERTY(EditDefaultsOnly, Category="Skill")
	USoundBase* StartSound;

	UPROPERTY(EditDefaultsOnly, Category="Skill")
	UNiagaraSystem* WireHitEffect;
	
	UPROPERTY(EditDefaultsOnly, Category="WireAction|Move")
	float InitialAcceleration = 6000.f;

	UPROPERTY(EditDefaultsOnly, Category="WireAction|Move")
	float MaxSpeed = 2500.f;

	// 이 거리 이내에서 가속 스케일 감소 시작
	UPROPERTY(EditDefaultsOnly, Category="WireAction|Move")
	float DecelStartDistance = 800.f;

	// 최소 가속 스케일
	UPROPERTY(EditDefaultsOnly, Category="WireAction|Move")
	float MinAccelScale = 0.2f;
	
	// 비행 종료 후에도 유지할 최대 속도
	UPROPERTY(EditDefaultsOnly, Category="WireAction|Move")
	float MaxKeepSpeed = 600.f;

	// 도달 판정 반경
	UPROPERTY(EditDefaultsOnly, Category="WireAction|Move")
	float AcceptRadius = 120.f;

	// 이동 틱 주기
	UPROPERTY(EditDefaultsOnly, Category="WireAction|Tick")
	float TickInterval = 0.016f;

	// 와이어 최대 사거리
	UPROPERTY(EditDefaultsOnly, Category="WireAction|Tick")
	float MaxWireDistance = 3000.f;


	// 케이블
	UPROPERTY(EditDefaultsOnly, Category="WireAction|Cable")
	TSubclassOf<ACableActor> CableActorClass;

	UPROPERTY()
	UFDGameplayStatic* GameplayStatics = nullptr;
protected:
	UFUNCTION()
	void LaunchWire(FGameplayEventData EventData);

	UFUNCTION()
	void OnInputReleased(float TimeHeld);

	void TickWireMove();

	// 케이블 생성/제거
	void SpawnCable(const FVector& StartWorldLoc, const FName& StartSocketName, const FVector& EndWorldLoc, USceneComponent* OptionalEndComp = nullptr, FName OptionalEndSocket = NAME_None);
	void DestroyCable();

	// 비행 시작/정리
	void BeginFlying();
	void EndFlying(bool bKeepMomentum);

	UFUNCTION()
	void LoopMontagePlay();

	// 내부 상태
	// End가 특정 컴포넌트/소켓에 부착된 경우 참조
	TWeakObjectPtr<USceneComponent> EndAttachComp;
	FName EndAttachSocketName = NAME_None;
	
	UPROPERTY()
	TWeakObjectPtr<ACableActor> CableActor;

	UPROPERTY()
	TWeakObjectPtr<AActor> CableEndAnchor;

	UPROPERTY()
	FVector TargetLocation = FVector::ZeroVector;

	UPROPERTY()
	FTimerHandle MoveTimerHandle;

	UPROPERTY()
	bool bMoving = false;

	// HangOn 트리거용
	UPROPERTY(EditDefaultsOnly, Category="WireAction")
	FGameplayTag HangOnEventTag;
	
	UPROPERTY()
	bool bCanTriggerHangOn = false;

	UPROPERTY()
	TWeakObjectPtr<AActor> HangOnTarget;

	UPROPERTY()
	FName HangOnAttachBoneName = NAME_None;

public:
	TWeakObjectPtr<AActor> GetHangOnTarget() const { return HangOnTarget; }
	FName GetHangOnBoneName() const { return HangOnAttachBoneName; }

	void SetHangOnTarget(AActor* InActor, FName InBoneName)
	{
		HangOnTarget = InActor;
		HangOnAttachBoneName = InBoneName;
	}
};
