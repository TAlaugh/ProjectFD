// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/Player/CharacterJumpAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "GameFramework/Character.h"
#include "GAS/GA/AT/Player/AT_JumpAndWaitForLanding.h"
#include "GAS/Player/PlayerAttributeSet.h"
#include "Kismet/GameplayStatics.h"


UCharacterJumpAbility::UCharacterJumpAbility()
{
}

bool UCharacterJumpAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
                                           const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	bool bResult = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
	if (!bResult)
	{
		return false;
	}

	const ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	return (Character && Character->CanJump());
}

void UCharacterJumpAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AFDPlayerCharacter* PC = Cast<AFDPlayerCharacter>(ActorInfo->AvatarActor.Get());
	PC->JumpMaxCount = FMath::Max(PC->JumpMaxCount, MaxJumpCount);
	PC->Jump();
	CurrentJumpCount = 1;
	PC->Multicast_PlaySkillSoundAtLocation(JumpSound, PC->GetActorLocation());
	PC->LandedDelegate.AddDynamic(this, &UCharacterJumpAbility::OnLandedCallback);
}

void UCharacterJumpAbility::InputPressed(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	AFDPlayerCharacter* PC = GetPlayerCharacterFromActorInfo();
	// 점프 가능하면 처리
	if (CurrentJumpCount < MaxJumpCount)
	{
		// 2번째 점프(이상)부터 전용 몽타주 재생
		if (CurrentJumpCount >= 1 && DoubleJumpMontage)
		{
			// AbilityTask로 안전하게 재생(능력 종료 시 정지)
			if (UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,DoubleJumpMontage,1,NAME_None,true))
			{
				Task->ReadyForActivation();
			}
		}
		GetPlayerCharacterFromActorInfo()->Multicast_PlaySkillSoundAtLocation(JumpSound, GetPlayerCharacterFromActorInfo()->GetActorLocation());
		PC->Jump();
		++CurrentJumpCount;
	}
}

void UCharacterJumpAbility::InputReleased(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	AFDPlayerCharacter* PC = GetPlayerCharacterFromActorInfo();
	PC->StopJumping();
}

void UCharacterJumpAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	// ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	// Character->LandedDelegate.RemoveDynamic(this, &UCharacterJumpAbility::OnLandedCallback);
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCharacterJumpAbility::OnLandedCallback(const FHitResult& Hit)
{
	UAbilitySystemComponent* ASC = GetFDAbilitySystemComponentFromActorInfo();
	const UPlayerAttributeSet* PlayerAttrSet = ASC->GetSet<UPlayerAttributeSet>();
	const float PlayerRadius = PlayerAttrSet->GetSkillRadius();
	const float PlayerPower = PlayerAttrSet->GetSkillDamage();
	float JumpFinalRadius = LandingDamageRadius * PlayerRadius;
	float JumpDamage = LandingDamage * PlayerPower;
	
	if (CurrentJumpCount > 1)
	{
		AFDPlayerCharacter* PC = Cast<AFDPlayerCharacter>(GetAvatarActorFromActorInfo());
		PC->Server_RequestLanding(Hit.TraceStart, Hit.ImpactPoint, JumpFinalRadius, JumpDamage, LandingNiagaraSystem);
		PC->Multicast_PlaySkillSoundAtLocation(SkillSound, PC->GetActorLocation());
	}
	GetPlayerCharacterFromActorInfo()->Multicast_PlaySkillSoundAtLocation(LandingSound, Hit.ImpactPoint);
	CurrentJumpCount = 0;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
