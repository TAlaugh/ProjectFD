// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/Player/HangOn/HangOnAttackAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "GAS/FDGameplayStatic.h"
#include "GAS/Player/PlayerAttributeSet.h"
#include "Kismet/GameplayStatics.h"


UHangOnAttackAbility::UHangOnAttackAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ActivationRequiredTags.AddTag(UFDGameplayStatic::GetOwnerHangOnTag());
}

void UHangOnAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayHangOnAttackMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, HangOnAttackMontage);
		PlayHangOnAttackMontageTask->OnCompleted.AddDynamic(this, &UHangOnAttackAbility::K2_EndAbility);
		PlayHangOnAttackMontageTask->OnBlendOut.AddDynamic(this, &UHangOnAttackAbility::K2_EndAbility);
		PlayHangOnAttackMontageTask->OnInterrupted.AddDynamic(this, &UHangOnAttackAbility::K2_EndAbility);
		PlayHangOnAttackMontageTask->OnCancelled.AddDynamic(this, &UHangOnAttackAbility::K2_EndAbility);
		PlayHangOnAttackMontageTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitHangOnAttackEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag(FName("Event.HangOn.Attack")), nullptr, false, true);
		WaitHangOnAttackEventTask->EventReceived.AddDynamic(this, &UHangOnAttackAbility::HangOnAttack);
		WaitHangOnAttackEventTask->ReadyForActivation();
	}
	
}

void UHangOnAttackAbility::HangOnAttack(FGameplayEventData EventData)
{
	AFDPlayerCharacter* PC = Cast<AFDPlayerCharacter>(GetAvatarActorFromActorInfo());
	AActor* OwnerActor = GetAvatarActorFromActorInfo();
	UAbilitySystemComponent* ASC = GetFDAbilitySystemComponentFromActorInfo();
	const UPlayerAttributeSet* PlayerAttrSet = ASC->GetSet<UPlayerAttributeSet>();
	const float HangOnAttackDamage = PlayerAttrSet->GetSkillDamage() *  HangOnAttackDamageMultiplier;

	if (!OwnerActor->HasAuthority())
	{
		return; // 서버에서만 처리
	}
	
	// 스피어 트레이스 파라미터
	const float TraceRange  = 10.f;   // 예시: 공격 거리
	const float TraceRadius = 10.f;    // 예시: 스피어 반경
	
	const FVector Forward = OwnerActor->GetActorForwardVector();
	const FVector Start   = OwnerActor->GetActorLocation() + Forward * 10.f; // 살짝 전방에서 시작
	const FVector End     = Start + Forward * TraceRange;
	
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(HangOnAttackTrace), false, OwnerActor);
	QueryParams.bReturnPhysicalMaterial = false;
	QueryParams.AddIgnoredActor(OwnerActor);
	
	TArray<FHitResult> Hits;
	const bool bHit = GetWorld()->SweepMultiByChannel(
		Hits,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,                               // 필요 시 커스텀 채널로 변경
		FCollisionShape::MakeSphere(TraceRadius),
		QueryParams
	);
	
	// Instigator \& DamageCauser 설정
	AController* InstigatorController = nullptr;
	if (APawn* PawnOwner = Cast<APawn>(OwnerActor))
	{
		InstigatorController = PawnOwner->GetController();
	}
	
	const FVector HitFromDirection = (End - Start).GetSafeNormal();

	TSet<AActor*> ProcessedActors; // 중복 제거용
	int32 AppliedCount = 0;
	
	for (FHitResult& Hit : Hits)
	{
		if (MaxHitCount > 0 && AppliedCount >= MaxHitCount)
		{
			break; // 최대 타격 수 도달
		}
		
		AActor* HitActor = Hit.GetActor();
		if (!HitActor || HitActor == OwnerActor)
		{
			continue;
		}

		// 이미 처리한 액터면 스킵
		if (ProcessedActors.Contains(HitActor))
		{
			continue;
		}
		ProcessedActors.Add(HitActor);
		
		Hit.bBlockingHit = true;
	
		UGameplayStatics::ApplyPointDamage(
			HitActor,
			HangOnAttackDamage,
			HitFromDirection,
			Hit,
			InstigatorController,
			OwnerActor,
			UDamageType::StaticClass()
		);
	}

	PC->Multicast_PlaySkillSoundAtLocation(HangOnAttackSound, Start);
	PC->Multicast_SpawnSkillNiagaraEffectAtLocation(HangOnAttackNiagaraEffect, Start, End);
	UE_LOG(LogTemp, Warning, TEXT("HangOnAttackAbility applied damage: %f"), HangOnAttackDamage);
}