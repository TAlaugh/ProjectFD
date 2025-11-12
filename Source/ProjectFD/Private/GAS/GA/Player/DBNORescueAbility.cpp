// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/Player/DBNORescueAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "Equipment/EquipmentActor.h"


UDBNORescueAbility::UDBNORescueAbility()
{
	InstancingPolicy   = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	DBNOStatusTag = FGameplayTag::RequestGameplayTag(FName("Player.Status.DBNO"), false);

}

void UDBNORescueAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 서버에서 트레이스가 돌아가므로 서버에서 바인드
	if (AActor* Avatar = GetAvatarActorFromActorInfo())
	{
		if (AFDPlayerCharacter* PC = Cast<AFDPlayerCharacter>(Avatar))
		{
			CachedPC = PC;
			PC->OnDetectedActor.AddDynamic(this, &UDBNORescueAbility::HandleDetectedActor);
		}
	}
}

void UDBNORescueAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (CachedPC.IsValid())
	{
		if (CachedPC->GetWeaponActor())
		{
			CachedPC->GetWeaponActor()->SetWeaponVisibility(true);
		}
		CachedPC->OnDetectedActor.RemoveDynamic(this, &UDBNORescueAbility::HandleDetectedActor);
		CachedPC.Reset();
	}
	CachedDetectedActor.Reset();
	bMontageStarted = false;
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UDBNORescueAbility::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}


void UDBNORescueAbility::OnRescueMontageCompleted()
{
	// 서버 권한에서만 태그 조작(복제 보장)
	if (!CurrentActorInfo || !CurrentActorInfo->IsNetAuthority())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
	
	// 루즈 태그 기준 제거(효과로 부여된 태그라면 해당 GE 제거 로직 필요)
	if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(CachedDetectedActor.Get()))
	{
		if (DBNOStatusTag.IsValid())
		{
			FGameplayTagContainer Granted;
			Granted.AddTag(DBNOStatusTag);
			TargetASC->RemoveActiveEffectsWithGrantedTags(Granted);

			UE_LOG(LogTemp, Log, TEXT("Removed tag %s from: %s"),
				*DBNOStatusTag.ToString(), *GetNameSafe(CachedDetectedActor.Get()));
		}
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}


void UDBNORescueAbility::OnRescueMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UDBNORescueAbility::HandleDetectedActor(AActor* DetectedActor)
{
	// 널 또는 중복 실행 방지
	if (!DetectedActor || bMontageStarted)
	{
		return;
	}

	// 전달받은 타겟 보관 및 처리
	CachedDetectedActor = DetectedActor;

	// 대상의 ASC에서 DBNO 태그 확인
	if (!DBNOStatusTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("DBNORescueAbility: DBNOStatusTag is invalid."));
		return;
	}

	UAbilitySystemComponent* TargetASC =
		UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(DetectedActor);

	if (!TargetASC || !TargetASC->HasMatchingGameplayTag(DBNOStatusTag))
	{
		return;
	}

	// 서버에서 감지되었으면, 소유 클라에도 동일 이벤트 알림(로컬 몽타주 시작 유도)
	if (CurrentActorInfo && CurrentActorInfo->IsNetAuthority())
	{
		if (AFDPlayerCharacter* PC = CachedPC.Get())
		{
			PC->ClientNotifyTargetDetected(DetectedActor);
		}
	}
	
	// 각 측(서버/소유클라)에서 로컬로 몽타주 시작
	if (UAbilityTask_PlayMontageAndWait* Task =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, RescueMontage))
	{
		Task->OnCompleted.AddDynamic(this, &UDBNORescueAbility::OnRescueMontageCompleted);
		Task->OnBlendOut.AddDynamic(this, &UDBNORescueAbility::OnRescueMontageCompleted);
		Task->OnInterrupted.AddDynamic(this, &UDBNORescueAbility::OnRescueMontageCancelled);
		Task->OnCancelled.AddDynamic(this, &UDBNORescueAbility::OnRescueMontageCancelled);
		Task->ReadyForActivation();

		bMontageStarted = true;
	}
	if (CachedPC->GetWeaponActor())
	{
		CachedPC->GetWeaponActor()->SetWeaponVisibility(false);
	}
}
