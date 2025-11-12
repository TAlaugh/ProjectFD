// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Player/PlayerGameplayAbility.h"
#include "HangOnAbility.generated.h"

class UCharacterMovementComponent;
class UWireActionAbility;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UHangOnAbility : public UPlayerGameplayAbility
{
	GENERATED_BODY()
	
public:
	UHangOnAbility();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	UAnimMontage* HangOnStartMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	UAnimMontage* HangOnLoopMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	UAnimMontage* HangOnEndMontage = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<UGameplayEffect> DurationGEClass;

	UPROPERTY(EditDefaultsOnly, Category="Skill")
	float HangOnDuration = 5.f;

	// 타겟에 부여할 태그 GE
	UPROPERTY(EditDefaultsOnly, Category="HangOn")
	TSubclassOf<UGameplayEffect> AttachPlayerPartsTagGEClass;

	UPROPERTY()
	UWireActionAbility* WireActionAbility = nullptr;

	UPROPERTY()
	TWeakObjectPtr<AActor> HangOnActor = nullptr;

	UPROPERTY()
	FName HangOnActorBoneName = NAME_None;
	
	UPROPERTY()
	AFDPlayerCharacter* PC = nullptr;
	
	UPROPERTY()
	USceneComponent* AttachTarget = nullptr;

	float AimStartArmLength = 0.f;
	
	UFUNCTION()
	void LoopHangOnMontagePlay();

	UFUNCTION()
	void AttachToHangOnBone();

	UFUNCTION()
	void DetachFromHangOnBone();
	

	FTimerHandle DurationTimerHandle;
	
	// 이동 모드 복구용
	TEnumAsByte<EMovementMode> PrevMovementMode = MOVE_Walking;
	uint8 PrevCustomMode = 0;
	
	// 매달 대상 스켈레탈 메시
	TWeakObjectPtr<USkeletalMeshComponent> HangOnTargetSkel;

	///////////////////////////////////////
	// 어빌리티 식별용 태그(어빌리티의 AbilityTags에 동일 태그가 있어야 함)
	UPROPERTY(EditDefaultsOnly, Category="HangOn")
	FGameplayTag HangOnAttackAbilityTag;
	FGameplayTag HangOnGrabAbilityTag;
	FGameplayTag HangOnGrabEventTag;

	// ASC 델리게이트 핸들
	FDelegateHandle OnAbilityEndedHandle;

	// 어빌리티 종료 감지 콜백
	UFUNCTION()
	void OnASCAbilityEnded(const FAbilityEndedData& Data);

	// 실시간 쿨다운(지속시간) 속도 제어 API \- 1.0이 기본, 2.0은 2배 빠르게, 0.5는 2배 느리게
	UFUNCTION(BlueprintCallable, Category="HangOn|Cooldown")
	void SetHangOnCooldownSpeed(float NewSpeed);

	// 적용된 지속효과(GE) 핸들 저장
	FActiveGameplayEffectHandle DurationEffectHandle;
	
	// 현재 속도 스케일(1.0이 기본)
	float HangOnTimeScale = 1.f;

	// 종료 타이머 재스케줄링
	void RescheduleEndTimer(float NewRemainingSeconds);

	// Grab 이벤트 태그 변화 델리게이트 핸들
	FDelegateHandle HangOnGrabTagChangedHandle;
	FDelegateHandle HangOnBodySwingTagChangedHandle;

	// Grab 이벤트 태그 변화 콜백
	UFUNCTION()
	void OnHangOnGrabTagChanged(const FGameplayTag Tag, int32 NewCount);
	UFUNCTION()
	void OnHangOnBodySwingTagChanged(const FGameplayTag Tag, int32 NewCount);

	UFUNCTION()
	void OnHangOnCooldownFromTags();

	// 타겟 ASC와 적용된 GE 핸들 캐싱
	TWeakObjectPtr<UAbilitySystemComponent> TargetASCWeak;
	FActiveGameplayEffectHandle TargetAttachTagGEHandle;

	// 작용/ 제거 헬퍼
	void ApplyAttachTagToTarget(AActor* TargetActor);
	void RemoveAttachTagFromTarget();

	///////////////////////////////// 로그
	// 쿨다운 로그 틱 타이머
	UPROPERTY(EditDefaultsOnly, Category="HangOn|Debug")
	float CooldownLogInterval = 0.1f;
	
	FTimerHandle CooldownLogTimerHandle;
	// 쿨다운 로그 틱 제어
	void StartCooldownLogTicker();
	void StopCooldownLogTicker();
	void LogCooldownTick();

protected:
	bool bCooldownLogTickerActive = false;
};
