// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/Player/BunnyPassiveSkill.h"

#include "AbilitySystemComponent.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "GAS/FDGameplayStatic.h"
#include "GAS/Player/PlayerAttributeSet.h"


UBunnyPassiveSkill::UBunnyPassiveSkill()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	bRetriggerInstancedAbility = false;
	bActivateOnGiveAbility = true;
}

void UBunnyPassiveSkill::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (!bActivateOnGiveAbility)
	{
		return;
	}
	
	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (!ASC)
	{
		return;
	}
	
	if (ActorInfo->IsNetAuthority() && !Spec.IsActive())
	{
		ASC->TryActivateAbility(Spec.Handle);
	}
}

void UBunnyPassiveSkill::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}
	
	AActor* Avatar = ActorInfo->AvatarActor.Get();
	LastLocation = Avatar->GetActorLocation();
	AccumulatedDistance = 0.0;
	
	StartTimer();
	StartElectricDecreaseTimer();
}

void UBunnyPassiveSkill::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	StopTimer();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBunnyPassiveSkill::StartTimer()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(DistanceTimerHandle, this, &UBunnyPassiveSkill::OnDistanceCheck, IntervalSec, true);
	}
}

void UBunnyPassiveSkill::StartElectricDecreaseTimer()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(ElectricDecreaseTimerHandle, this, &UBunnyPassiveSkill::OnElectricDecreaseCheck, IntervalSec, true);
	}
}

void UBunnyPassiveSkill::StopTimer()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DistanceTimerHandle);
		World->GetTimerManager().ClearTimer(ElectricDecreaseTimerHandle);
	}
}

void UBunnyPassiveSkill::OnElectricDecreaseCheck()
{
	if (FPlatformTime::Seconds() - LastElectricIncreaseTime > ElectricDecreaseDelay)
	{
		ElectricDecrease();
	}
}

void UBunnyPassiveSkill::OnDistanceCheck()
{
	// 객체 자체가 유효하지 않으면 타이머 정지
	if (!IsValid(this))
	{
		StopTimer();
		return;
	}
	
	const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		StopTimer();
		return;
	}

	AActor* Avatar = ActorInfo->AvatarActor.Get();
	if (!IsValid(Avatar))
	{
		StopTimer();
		return;
	}
	
	const FVector CurrentLocation = Avatar->GetActorLocation();

	const float DistanceMoved = FVector::Dist(LastLocation, CurrentLocation);
	
	LastLocation = CurrentLocation;
	
	AccumulatedDistance += DistanceMoved;
	AccumulatedDistance2 += DistanceMoved;

	const int32 Increments = FMath::FloorToInt(AccumulatedDistance / IntervalDistance);
	int32 Increments2 = FMath::FloorToInt(AccumulatedDistance2 / IntervalDistance);

	if (Increments2 > 1)
	{
	if (!GetPlayerCharacterFromActorInfo()->GetCharacterMovement()->IsFalling() && HasAuthority(&CurrentActivationInfo))
	{
		GetPlayerCharacterFromActorInfo()->Multicast_PlaySkillSoundAtLocation(RunningSound, CurrentLocation);
	}
		AccumulatedDistance2 = 0.0f;
	}
	
	if (Increments > 0)
	{
		AccumulatedDistance -= Increments * IntervalDistance;
	
		const float Gain = Increments * ElectricPerIncrease;
	
		UAbilitySystemComponent* ASC = GetFDAbilitySystemComponentFromActorInfo();
		const FGameplayEffectContextHandle GEContextHandle = ASC->MakeEffectContext();
		const FGameplayEffectSpecHandle ElectricSpecHandle = ASC->MakeOutgoingSpec(ElectricIncreaseGEClass, 1.f, GEContextHandle);
		const FGameplayTag ElectricTag = FGameplayTag::RequestGameplayTag(TEXT("Data.Bunny.Passive.Electric"));

		ElectricSpecHandle.Data->SetSetByCallerMagnitude(ElectricTag, Gain);
		ASC->ApplyGameplayEffectSpecToSelf(*ElectricSpecHandle.Data.Get());
		LastElectricIncreaseTime = FPlatformTime::Seconds();
	}
}

void UBunnyPassiveSkill::ElectricDecrease()
{
	UAbilitySystemComponent* ASC = GetFDAbilitySystemComponentFromActorInfo();
	const FGameplayEffectContextHandle GEContextHandle = ASC->MakeEffectContext();
	const FGameplayEffectSpecHandle ElectricSpecHandle = ASC->MakeOutgoingSpec(ElectricDecreaseGEClass, 1.f, GEContextHandle);

	// 감소량은 필요에 따라 조절
	const FGameplayTag ElectricTag = FGameplayTag::RequestGameplayTag(TEXT("Data.Bunny.Passive.Electric"));
	ElectricSpecHandle.Data->SetSetByCallerMagnitude(ElectricTag, -ElectricPerDecrease);
	ASC->ApplyGameplayEffectSpecToSelf(*ElectricSpecHandle.Data.Get());
}
