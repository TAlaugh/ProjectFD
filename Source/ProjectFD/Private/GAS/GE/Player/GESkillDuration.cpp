// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GE/Player/GESkillDuration.h"


UGESkillDuration::UGESkillDuration()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DurationMagnitude = FScalableFloat(5.0f);
	
}
