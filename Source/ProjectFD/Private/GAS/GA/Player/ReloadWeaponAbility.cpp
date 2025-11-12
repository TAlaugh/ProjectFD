// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/Player/ReloadWeaponAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "Equipment/EquipmentActor.h"
#include "GameFramework/Character.h"
#include "GAS/FDAbilitySystemComponent.h"
#include "GAS/Player/PlayerAttributeSet.h"


UReloadWeaponAbility::UReloadWeaponAbility()
{
}

void UReloadWeaponAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	ASC = Cast<UFDAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());

	
	const UPlayerAttributeSet* PlayerAttrSet = ASC->GetSet<UPlayerAttributeSet>();
	if (PlayerAttrSet->GetGeneralAmmo() <= 0 )
	{
		UE_LOG(LogTemp, Warning, TEXT("소지하고 있는 탄약이 없습니다."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	if (PlayerAttrSet->GetAmmo() >= PlayerAttrSet->GetMaxAmmo())
	{
		UE_LOG(LogTemp, Warning, TEXT("탄창이 가득 찼습니다."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	// 장착 무기 찾기(예시: 부착 액터에서 탐색)
	AEquipmentActor* Equipment = nullptr;
	{
		TArray<AActor*> Attached;
		Character->GetAttachedActors(Attached);
		for (AActor* A : Attached)
		{
			if ((Equipment = Cast<AEquipmentActor>(A)) != nullptr)
			{
				break;
			}
		}
	}
	AFDPlayerCharacter* PC = Cast<AFDPlayerCharacter>(GetAvatarActorFromActorInfo());
	FGameplayTag AmmoType = PC->GetActiveWeaponAmmoType();
	if (HasAuthority(&CurrentActivationInfo))
	{
		if (AmmoType.MatchesTagExact(FGameplayTag::RequestGameplayTag("Equipment.AmmoType.General")))
		{
			PC->Multicast_PlaySkillSoundAtLocation(MGReloadSound, Equipment->GetActorLocation());
		}
		else
		{
			PC->Multicast_PlaySkillSoundAtLocation(LNCReloadSound, Equipment->GetActorLocation());
		}
	}
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlaySkillMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ReloadMontage);
		PlaySkillMontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlaySkillMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlaySkillMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlaySkillMontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlaySkillMontageTask->ReadyForActivation();
	}

	UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag(FName("Event.Weapon.Reload")), nullptr, false, true);
	if (WaitEventTask)
	{
		WaitEventTask->EventReceived.AddDynamic(this, &ThisClass::OnMontageEnded);
		WaitEventTask->ReadyForActivation();
	}
}

void UReloadWeaponAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UReloadWeaponAbility::OnMontageEnded(FGameplayEventData Payload)
{
	AFDPlayerCharacter* PC = Cast<AFDPlayerCharacter>(GetAvatarActorFromActorInfo());
	const UPlayerAttributeSet* PlayerAttrSet = ASC->GetSet<UPlayerAttributeSet>();
	float CurrentAmmo = PlayerAttrSet->GetAmmo();
	float MaxAmmo = PlayerAttrSet->GetMaxAmmo();
	float GeneralAmmo = PlayerAttrSet->GetGeneralAmmo();
	float HighPowerAmmo = PlayerAttrSet->GetHighPowerAmmo();
	float SpecialAmmo = PlayerAttrSet->GetSpecialAmmo();
	float ImpactAmmo = PlayerAttrSet->GetImpactAmmo();
	
	FGameplayAttribute GeneralAmmoAttribute = UPlayerAttributeSet::GetGeneralAmmoAttribute();
	FGameplayAttribute HighPowerAmmoAttribute = UPlayerAttributeSet::GetHighPowerAmmoAttribute();
	FGameplayAttribute CurrentAmmoAttribute = UPlayerAttributeSet::GetAmmoAttribute();
	FGameplayAttribute SpecialAmmoAttribute = UPlayerAttributeSet::GetSpecialAmmoAttribute();
	FGameplayAttribute ImpactAmmoAttribute = UPlayerAttributeSet::GetImpactAmmoAttribute();
	

	FGameplayTag AmmoType = PC->GetActiveWeaponAmmoType();

	float FinalAmmo = GeneralAmmo;
	FGameplayAttribute FinalAmmoAttribute = GeneralAmmoAttribute;
	
	if (AmmoType.MatchesTagExact(FGameplayTag::RequestGameplayTag("Equipment.AmmoType.HighPower")))
	{
		FinalAmmo = HighPowerAmmo;
		FinalAmmoAttribute = HighPowerAmmoAttribute;
	}
	else if (AmmoType.MatchesTagExact(FGameplayTag::RequestGameplayTag("Equipment.AmmoType.General")))
	{
		FinalAmmo = GeneralAmmo;
		FinalAmmoAttribute = GeneralAmmoAttribute;
	}
	else if (AmmoType.MatchesTagExact(FGameplayTag::RequestGameplayTag("Equipment.AmmoType.Special")))
	{
		FinalAmmo = SpecialAmmo;
		FinalAmmoAttribute = SpecialAmmoAttribute;
	}
	else if (AmmoType.MatchesTagExact(FGameplayTag::RequestGameplayTag("Equipment.AmmoType.Impact")))
	{
		FinalAmmo = ImpactAmmo;
		FinalAmmoAttribute = ImpactAmmoAttribute;
	}
	
	if ( FinalAmmo < CurrentAmmo && (FinalAmmo + CurrentAmmo) > MaxAmmo )
	{
		ASC->ApplyModToAttributeUnsafe(CurrentAmmoAttribute, EGameplayModOp::Additive, MaxAmmo - CurrentAmmo);
		ASC->ApplyModToAttributeUnsafe(FinalAmmoAttribute, EGameplayModOp::Override, FinalAmmo - (MaxAmmo - CurrentAmmo));
	}
	else if ( (FinalAmmo + CurrentAmmo) <= MaxAmmo )
	{
		ASC->ApplyModToAttributeUnsafe(CurrentAmmoAttribute, EGameplayModOp::Additive, FinalAmmo);
		ASC->ApplyModToAttributeUnsafe(FinalAmmoAttribute, EGameplayModOp::Additive, -FinalAmmo);
	}
	else
	{
		ASC->ApplyModToAttributeUnsafe(CurrentAmmoAttribute, EGameplayModOp::Override, MaxAmmo);
		ASC->ApplyModToAttributeUnsafe(FinalAmmoAttribute, EGameplayModOp::Additive, CurrentAmmo - MaxAmmo);
	}
	if (HasAuthority(&CurrentActivationInfo))
	{
		if (AmmoType.MatchesTagExact(FGameplayTag::RequestGameplayTag("Equipment.AmmoType.General")))
		{
			PC->Multicast_PlaySkillSoundAtLocation(MGReloadEndSound, PC->GetActorLocation());
		}
		else
		{
			PC->Multicast_PlaySkillSoundAtLocation(LNCReloadEndSound, PC->GetActorLocation());
		}
	}
	//EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
