// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/Player/AttackHitCheckAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/Player/PlayerAttributeSet.h"
#include "GAS/GA/AT/Player/AT_Trace.h"
#include "GAS/GA/TA/TA_Trace.h"
#include "Kismet/GameplayStatics.h"


UAttackHitCheckAbility::UAttackHitCheckAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UAttackHitCheckAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UE_LOG(LogTemp, Warning, TEXT("ActivateAbility UAttackHitCheckAbility"));

	UAT_Trace* AttackTraceTask = UAT_Trace::CreateTask(this, ATA_Trace::StaticClass());
	AttackTraceTask->OnComplete.AddDynamic(this, &UAttackHitCheckAbility::OnTraceResultCallback);
	AttackTraceTask->ReadyForActivation();
}

void UAttackHitCheckAbility::OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	// 서버에서만 데미지 적용(중복 방지)
	const FGameplayAbilityActorInfo* Info = GetCurrentActorInfo();
	const bool bIsServer = (Info && Info->IsNetAuthority());

	if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetDataHandle, 0))
	{
		const FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 0);
		AActor* HitActor = HitResult.GetActor();
		if (HitActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor : %s"), *HitActor->GetName());

			// 소스 ASC와 데미지 값
			UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();
			const UPlayerAttributeSet* SourceAttribute = SourceASC ? SourceASC->GetSet<UPlayerAttributeSet>() : nullptr;
			const float Damage = SourceAttribute ? SourceAttribute->GetWeaponDamage() : 0.f;

			if (bIsServer && Damage > 0.f)
			{
				// 가해자/인스티게이터 설정
				AActor* DamageCauser = Info->AvatarActor.Get();
				AController* InstigatorController = nullptr;
				if (APawn* Pawn = Cast<APawn>(DamageCauser))
				{
					InstigatorController = Pawn->GetController();
				}

				UGameplayStatics::ApplyDamage(
					HitActor,
					Damage,
					InstigatorController,
					DamageCauser,
					UDamageType::StaticClass()
				);
			}
		}
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}