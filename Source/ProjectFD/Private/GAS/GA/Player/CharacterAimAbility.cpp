// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/Player/CharacterAimAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GAS/Player/PlayerAbilitySystemComponent.h"


UCharacterAimAbility::UCharacterAimAbility()
{
}

void UCharacterAimAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	// 상태 저장(나중에 EndAbility 호출용)
	AFDPlayerCharacter* PC = GetPlayerCharacterFromActorInfo();
	
	OriginalArmLength = PC->CameraBoom->TargetArmLength;
	
	const float NewTarget = OriginalArmLength - 175.f;
	StartAimInterp(NewTarget, AimInterpDuration);

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayAimMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AimInMontage, 1.f, NAME_None);
		PlayAimMontageTask->ReadyForActivation();
	}
}

void UCharacterAimAbility::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	
	// 릴리즈하면 원래 길이로 보간하고, 보간 완료 시 Ability 종료하도록 플래그 설정
	StartAimInterp(OriginalArmLength, AimInterpDuration);
	bEndOnInterpComplete = true;
}

void UCharacterAimAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCharacterAimAbility::StartAimInterp(float NewTarget, float Duration)
{
	AFDPlayerCharacter* PC = GetPlayerCharacterFromActorInfo();

	// 초기값 설정
	AimStartArmLength = PC->CameraBoom->TargetArmLength;
	AimTargetArmLength = NewTarget;
	AimInterpElapsed = 0.f;
	AimInterpDuration = FMath::Max(0.01f, Duration);
	bIsAiming = true;

	// 타이머로 Tick 호출
	FTimerDelegate TimerDel = FTimerDelegate::CreateUObject(this, &UCharacterAimAbility::TickAimInterp);
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(AimInterpTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(AimInterpTimerHandle, TimerDel, 0.01f, true);
	}
}

void UCharacterAimAbility::TickAimInterp()
{
	if (!bIsAiming) return;
	AFDPlayerCharacter* PC = GetPlayerCharacterFromActorInfo();
	if (!PC || !PC->CameraBoom || !GetWorld())
	{
		StopAimInterp();
		return;
	}

	const float Delta = GetWorld()->GetDeltaSeconds();
	AimInterpElapsed += Delta;
	const float Alpha = FMath::Clamp(AimInterpElapsed / AimInterpDuration, 0.f, 1.f);

	// 부드러운 보간 (SmoothStep 적용)
	const float SmoothAlpha = FMath::SmoothStep(0.f, 1.f, Alpha);
	const float NewLength = FMath::Lerp(AimStartArmLength, AimTargetArmLength, SmoothAlpha);

	PC->CameraBoom->TargetArmLength = NewLength;
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (Alpha >= 1.f)
	{	
		StopAimInterp();
		
		// 보간 완료 후 Ability 종료가 예약되어 있으면 종료
		if (bEndOnInterpComplete)
		{
			UAbilityTask_PlayMontageAndWait* PlayAimMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AimInMontage, 1.f, NAME_None);
			PlayAimMontageTask->ReadyForActivation();

			bEndOnInterpComplete = false;
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		}
	}
}

void UCharacterAimAbility::StopAimInterp()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(AimInterpTimerHandle);
	}
	bIsAiming = false;
}