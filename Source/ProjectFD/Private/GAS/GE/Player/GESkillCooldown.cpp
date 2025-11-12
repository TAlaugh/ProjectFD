// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GE/Player/GESkillCooldown.h"


UGESkillCooldown::UGESkillCooldown()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DurationMagnitude = FScalableFloat(5.0f);
	
	// const FGameplayTag CooldownTag = FGameplayTag::RequestGameplayTag(FName("Data.Skill.Cooldown"));
	// InheritableOwnedTagsContainer.AddTag(CooldownTag);

	// // 스택 처리: 하나만 유지, 재적용 시 남은 시간 갱신
	// StackingType = EGameplayEffectStackingType::AggregateBySource;
	// StackLimitCount = 1;
	// StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	// StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
}