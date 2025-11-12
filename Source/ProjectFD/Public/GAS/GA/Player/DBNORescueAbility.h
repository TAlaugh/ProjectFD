// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Player/PlayerGameplayAbility.h"
#include "DBNORescueAbility.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFD_API UDBNORescueAbility : public UPlayerGameplayAbility
{
	GENERATED_BODY()
	
public:
	UDBNORescueAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	UAnimMontage* RescueMontage = nullptr;

	UFUNCTION()
	void OnRescueMontageCompleted();
	UFUNCTION()
	void OnRescueMontageCancelled();
	
	// 캐릭터에서 넘어오는 감지 대상 수신
	UFUNCTION()
	void HandleDetectedActor(AActor* DetectedActor);

	// 바인드 해제를 위한 보관
	TWeakObjectPtr<AFDPlayerCharacter> CachedPC;

	// 필요 시 이후 로직에서 사용할 타겟 캐시
	TWeakObjectPtr<AActor> CachedDetectedActor;
	
	// 중복 실행 방지
	bool bMontageStarted = false;

	// DBNO 상태 태그(프로젝트 태그에 맞게 변경 가능)
	UPROPERTY(EditDefaultsOnly, Category = "Rescue|Tags")
	FGameplayTag DBNOStatusTag;
};




/* DBNO 구조 설정
 * 캐릭터 앞으로 라인트레이스를 쏴서 구조 가능한지 확인
 * 구조 가능하면 F키를 누르면 애니메이션 재생 끝날때까지 유지
 */