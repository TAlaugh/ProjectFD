// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Player/PlayerAbilitySystemComponent.h"

#include "DataAssets/StartUpData/DA_FDGameplayAbilities.h"

#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Equipment/EquipmentManagerComponent.h"
#include "GAS/FDGameplayStatic.h"
#include "GAS/Player/PlayerAttributeSet.h"
#include "GAS/Player/PlayerGameplayAbility.h"

void UPlayerAbilitySystemComponent::ApplyInitializeEffects()
{
	// 서버에서만 실행
	if (!GetOwner() || !GetOwner()->HasAuthority())
		return;
	
	for ( const TSubclassOf<UGameplayEffect>& GEClass : GAData->InitialEffects )
	{
		// GE 스펙 핸들 생성
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(GEClass, 1, MakeEffectContext());
		// 자기자신에게 GE 적용
		ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}
}

void UPlayerAbilitySystemComponent::OperatingInitializedAbilities()
{
	// 서버에서만 수행
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	// 서버
	for (const TSubclassOf<UGameplayAbility>& GAClass : GAData->InitializedAbilities)
	{
		FGameplayAbilitySpec Spec = FGameplayAbilitySpec(GAClass, 1, -1, nullptr);
		// 어빌리티 부여 및 한번만 실행
		GiveAbilityAndActivateOnce(Spec, nullptr);
	}

	for (const TPair<EFDAbilityInputID, TSubclassOf<UGameplayAbility>>& GAClassPair : GAData->BaseAbilities)
	{
		GiveAbility(FGameplayAbilitySpec(GAClassPair.Value, 0, (int32)GAClassPair.Key, nullptr));
	}

	for (const TPair<EFDAbilityInputID, TSubclassOf<UGameplayAbility>>& GAClassPair : GAData->SkillAbilities)
	{
		// InLevel을 0으로 세팅하는 이유 ( 초기상태이므로 스킬 해금이 되지 않음을 판단하는 수단으로 사용가능 )
		GiveAbility(FGameplayAbilitySpec(GAClassPair.Value, 0, (int32)GAClassPair.Key, nullptr));
	}
}
void UPlayerAbilitySystemComponent::AddEquipmentEffects(FFDEquipmentEntry* EquipmentEntry)
{
	FStreamableManager& Manager = UAssetManager::GetStreamableManager();
	TWeakObjectPtr<UPlayerAbilitySystemComponent> WeakThis(this);
	const FGameplayEffectContextHandle ContextHandle = MakeEffectContext();
	
	for (const FEquipmentStatEffectGroup& StatEffect : EquipmentEntry->EffectPackage.StatEffects)
	{
		if (IsValid(StatEffect.EffectClass.Get()))
		{
			const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(StatEffect.EffectClass.Get(), StatEffect.CurrentValue, ContextHandle);
			const FActiveGameplayEffectHandle ActiveHandle = ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

			EquipmentEntry->GrantedHandles.AddEffectHandle(ActiveHandle);
		}
		else
		{
			Manager.RequestAsyncLoad(StatEffect.EffectClass.ToSoftObjectPath(),
				[WeakThis, StatEffect, ContextHandle, EquipmentEntry]
				{
					const FGameplayEffectSpecHandle SpecHandle = WeakThis->MakeOutgoingSpec(StatEffect.EffectClass.Get(), StatEffect.CurrentValue, ContextHandle);
					const FActiveGameplayEffectHandle ActiveHandle = WeakThis->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

					EquipmentEntry->GrantedHandles.AddEffectHandle(ActiveHandle);
				});
		}
	}
}

void UPlayerAbilitySystemComponent::RemoveEquipmentEffects(FFDEquipmentEntry* EquipmentEntry)
{
	for (auto HandleIt = EquipmentEntry->GrantedHandles.ActiveEffects.CreateIterator(); HandleIt; ++HandleIt)
	{
		RemoveActiveGameplayEffect(*HandleIt);
		HandleIt.RemoveCurrent();
	}
}

void UPlayerAbilitySystemComponent::AddEquipmentAbilities(FFDEquipmentEntry* EquipmentEntry)
{
	FStreamableManager& Manager = UAssetManager::GetStreamableManager();
	TWeakObjectPtr<UPlayerAbilitySystemComponent> WeakThis(this);

	// Remove Abilities and Cache Specs 
	if (!EquipmentEntry->RemoveAbilityTags.IsEmpty())
	{
		TArray<FGameplayAbilitySpec> CopiedAbilities = GetActivatableAbilities();
		for (FGameplayAbilitySpec SpecIt : CopiedAbilities)
		{
			if (SpecIt.Ability->AbilityTags.HasAny(EquipmentEntry->RemoveAbilityTags))
			{
				EquipmentEntry->GrantedHandles.RemovedAbilitySpecs.Add(SpecIt);
				ClearAbility(SpecIt.Handle);
			}
		}
	}

	for (const FEquipmentAbilityGroup& AbilityToGrant : EquipmentEntry->EffectPackage.Abilities)
	{
		if (IsValid(AbilityToGrant.AbilityClass.Get()))
		{
			EquipmentEntry->GrantedHandles.GrantedAbilities.AddUnique(GrantEquipmentAbility(AbilityToGrant));
		}
		else
		{
			Manager.RequestAsyncLoad(AbilityToGrant.AbilityClass.ToSoftObjectPath(),
				[WeakThis, EquipmentEntry, AbilityToGrant]
				{
					EquipmentEntry->GrantedHandles.GrantedAbilities.AddUnique(WeakThis->GrantEquipmentAbility(AbilityToGrant));
				});
		}
	}
}

void UPlayerAbilitySystemComponent::RemoveEquipmentAbilities(FFDEquipmentEntry* EquipmentEntry)
{
	for (auto HandleIt = EquipmentEntry->GrantedHandles.GrantedAbilities.CreateIterator(); HandleIt; ++HandleIt)
	{
		ClearAbility(*HandleIt);
		HandleIt.RemoveCurrent();
	}

	for (FGameplayAbilitySpec SpecIt : EquipmentEntry->GrantedHandles.RemovedAbilitySpecs)
	{
		GiveAbility(SpecIt);
	}
}

FGameplayAbilitySpecHandle UPlayerAbilitySystemComponent::GrantEquipmentAbility(const FEquipmentAbilityGroup& AbilityGroup)
{
	FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec();
	if (AbilityGroup.AbilityInputID != EFDAbilityInputID::None)
	{
		AbilitySpec = FGameplayAbilitySpec(AbilityGroup.AbilityClass.Get(), AbilityGroup.AbilityLevel, (int32)AbilityGroup.AbilityInputID, nullptr);
	}
	else
	{
		AbilitySpec = FGameplayAbilitySpec(AbilityGroup.AbilityClass.Get(), AbilityGroup.AbilityLevel);
	}

	// if (UPlayerGameplayAbility* PlayerAbility = Cast<UPlayerGameplayAbility>(AbilitySpec.Ability))
	// {
	// 	AbilitySpec.DynamicAbilityTags.AddTag(PlayerAbility->InputTag);
	// }

	// Projectile 관련 Ability
	// if (UProjectileAbility* ProjectileAbility = Cast<UProjectileAbility>(AbilitySpec.Ability))
	// {
	// 	ProjectileAbility->ProjectileToSpawnTag = AbilityGroup.ContextTag;
	// }

	return GiveAbility(AbilitySpec);
}

UPlayerAbilitySystemComponent::UPlayerAbilitySystemComponent()
{
	GetGameplayAttributeValueChangeDelegate(UPlayerAttributeSet::GetHealthAttribute()).AddUObject(this, &UPlayerAbilitySystemComponent::HealthUpdated);
}

void UPlayerAbilitySystemComponent::HealthUpdated(const FOnAttributeChangeData& OnAttributeChangeData)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !GAData) return;

	// HP가 0 초과면 처리 불필요
	if (OnAttributeChangeData.NewValue > 0.f) return;

	// 이미 사망 상태면 중복 처리 방지
	if (HasMatchingGameplayTag(UFDGameplayStatic::GetDeadStatusTag()))
		return;

	// DBNO 상태가 아니면 DBNO 부여
	if (!HasMatchingGameplayTag(UFDGameplayStatic::GetDBNOStatusTag()))
	{
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(GAData->DBNOEffect, 1, MakeEffectContext());
		DBNOEffectHandle = ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
		return;
	}

	// DBNO 상태면 제거 후 사망 효과 적용
	if (DBNOEffectHandle.IsValid())
	{
		RemoveActiveGameplayEffect(DBNOEffectHandle);
		DBNOEffectHandle.Invalidate();
	}
	
	if (GAData->DeadEffect)
	{
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(GAData->DeadEffect, 1, MakeEffectContext());
		ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}
}

void UPlayerAbilitySystemComponent::ApplyFullStatEffect()
{
	AuthApplyGameplayEffect(GAData->InitialEffects[1], 1);
}

void UPlayerAbilitySystemComponent::AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> GameplayEffect, int Level)
{
	if (GetOwner()->HasAuthority())
	{
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(GameplayEffect, Level, MakeEffectContext());
		ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}
}