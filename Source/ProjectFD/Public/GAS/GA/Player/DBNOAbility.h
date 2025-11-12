// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Player/PlayerGameplayAbility.h"
#include "DBNOAbility.generated.h"

class UAbilityTask_WaitDelay;
class UAbilityTask_PlayMontageAndWait;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UDBNOAbility : public UPlayerGameplayAbility
{
	GENERATED_BODY()
	
public:
	UDBNOAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	UAnimMontage* DBNOStartMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	UAnimMontage* DBNOLoopMontage = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<UGameplayEffect> CostGEClass;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float DBNOHealthDrainPerSecond = 4.f;

	// DBNO 체력 소모 GE 핸들(EndAbility에서 제거)
	FActiveGameplayEffectHandle DBNOCostGEHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	USoundBase* DBNOStartSound;
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	USoundBase* DBNOLoopSound;
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	USoundBase* DBNOEndSound;
	
	UFUNCTION()
	void OnStartDBNO();

	UFUNCTION()
	void OnDeath();

	UFUNCTION()
	void NextMontage(FGameplayEventData Payload);

	UFUNCTION()
	void PlayDBNOSoundTick();

	FTimerHandle DBNOLoopSoundTimer;
	
};

/* DBNO 설정
 * 플레이어 체력이 0이 되면 DBNO 상태로 전환
 * DBNO 상태 전환은 태그로 전달, 플레이어가 태그를 받으면 AnimBP에서 애님레이어를 DBNO용으로 전환
 * 이동속도는 50이 되고 이동 가능, 화면 고정 해제, 카메라 회전 가능
 * 캐릭터가 DBNO 상태가 되면, 체력을 일시적으로 100퍼센트 채워주고, 초당 체력 4퍼씩 감소, 0이 되면 최종적으로 사망 ( Death )
 * 다른 플레이어가 DBNO 상태의 플레이어를 G키를 5초간 눌러 유지하면 부활시킬 수 있음 ( 애니메이션이 5초간 재생되고 성공적으로 끝까지 재생되면 부활 )
 *
 */
