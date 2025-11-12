// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/Player/BunnySkillThreeAbility.h"

#include "Engine/World.h"
#include "TimerManager.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "GAS/FDGameplayStatic.h"
#include "GAS/Player/PlayerAttributeSet.h"
#include "Kismet/GameplayStatics.h"


UBunnySkillThreeAbility::UBunnySkillThreeAbility()
{
	ActivationRequiredTags.AddTag(UFDGameplayStatic::GetOwnerCombatTag());
	CooldownTag = FGameplayTag::RequestGameplayTag(FName("Data.Skill.Cooldown.3"));
}

bool UBunnySkillThreeAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;

	// 쿨다운 태그 직접 검사 & 로그
	if (ASC)
	{
		float Remaining = 0.f, Total = 0.f;
		if (UFDGameplayStatic::TryGetCooldownTimeRemaining(ASC, CooldownTag, Remaining, Total))
		{
			UE_LOG(LogTemp, Warning, TEXT("쿨타임 중: %.2f / %.2f"), Remaining, Total);
			return false;
		}
	}
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UBunnySkillThreeAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);


	AActor* OwnerActor = GetOwningActorFromActorInfo();
	LastLocation = OwnerActor->GetActorLocation();
	AccumulatedDistance = 0.f;
	
	UAbilitySystemComponent* ASC = GetFDAbilitySystemComponentFromActorInfo();
	const UPlayerAttributeSet* PlayerAttrSet = ASC->GetSet<UPlayerAttributeSet>();

	
	// 비용 계산 & 자원 체크
	const float BunnySkillThreeCost = CostAmount * PlayerAttrSet->GetSkillCost();
	if (PlayerAttrSet->GetSpecialResource() < BunnySkillThreeCost )
	{
		UE_LOG(LogTemp, Warning, TEXT("전기력이 부족합니다."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// Effect Context + Spec 생성
	const FGameplayEffectContextHandle GEContextHandle = ASC->MakeEffectContext();
	const FGameplayEffectSpecHandle CostSpecHandle = ASC->MakeOutgoingSpec(CostGEClass, 1.f, GEContextHandle);
	const FGameplayEffectSpecHandle CooldownSpecHandle = ASC->MakeOutgoingSpec(CooldownGEClass, 1.f, GEContextHandle);
	const FGameplayEffectSpecHandle DurationSpecHandle = ASC->MakeOutgoingSpec(DurationGEClass, 1.f, GEContextHandle);

	// 쿨다운 지속시간 설정
	const float BunnySkillThreeCooldown = SkillCooldown * PlayerAttrSet->GetSkillCooldown();
	CooldownSpecHandle.Data->SetDuration(BunnySkillThreeCooldown, true);
	CooldownSpecHandle.Data->DynamicGrantedTags.AddTag(CooldownTag);

	// SetByCaller를 이용해 CostAmount 설정
	const FGameplayTag CostTag = FGameplayTag::RequestGameplayTag(TEXT("Data.Skill.Cost"));
	CostSpecHandle.Data->SetSetByCallerMagnitude(CostTag, -BunnySkillThreeCost);

	// 어빌리티 지속시간 설정
	const float BunnySkillThreeDuration = SkillDuration * PlayerAttrSet->GetSkillDuration();
	DurationSpecHandle.Data->SetDuration(BunnySkillThreeDuration, true);

	if (!CommitAbility(Handle,ActorInfo,ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo,true, false);
		return;
	}
	
	// GE 적용
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		ASC->ApplyGameplayEffectSpecToSelf(*CooldownSpecHandle.Data.Get());
		ASC->ApplyGameplayEffectSpecToSelf(*CostSpecHandle.Data.Get());
		ASC->ApplyGameplayEffectSpecToSelf(*DurationSpecHandle.Data.Get());
	}

	AFDPlayerCharacter* PC = Cast<AFDPlayerCharacter>(GetAvatarActorFromActorInfo());
	
	// 0.1초 뒤에 쿨다운 알림 실행
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(CooldownTimerHandle, [this]()
			{
				AFDPlayerCharacter* PC = Cast<AFDPlayerCharacter>(GetAvatarActorFromActorInfo());
				if (HasAuthority(&CurrentActivationInfo))
				{
					PC->NotifySkillCoolDown(CooldownTag);
				}
			},
			0.1f,
			false);
	}
	
	// 이동 기반 주기적 체크 시작
	StartTimer();
	
	// 지속 시간 후 어빌리티 종료
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			DurationTimerHandle,
			this,
			&UBunnySkillThreeAbility::OnAbilityDurationEnd,
			BunnySkillThreeDuration,
			false);
	}
}

void UBunnySkillThreeAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	StopTimer();
	
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DurationTimerHandle);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBunnySkillThreeAbility::StartTimer()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(DistanceTimerHandle, this, &UBunnySkillThreeAbility::OnDistanceCheck, IntervalSec, true);
	}
}

void UBunnySkillThreeAbility::StopTimer()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DistanceTimerHandle);
	}
}

void UBunnySkillThreeAbility::OnDistanceCheck()
{
	const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();
	AActor* Avatar = ActorInfo->AvatarActor.Get();
	AFDPlayerCharacter* PC = Cast<AFDPlayerCharacter>(GetAvatarActorFromActorInfo());
	const FVector CurrentLocation = Avatar->GetActorLocation();

	const float DistanceMoved = FVector::Dist(LastLocation, CurrentLocation);
	
	LastLocation = CurrentLocation;
	
	AccumulatedDistance += DistanceMoved;

	const int32 Increments = FMath::FloorToInt(AccumulatedDistance / MovementDistanceInterval);

	if (Increments <= 0)
	{
		return;
	}
	if (Increments > 0)
	{
		PerformMovementTriggeredAttack();
		SpawnSplineAtLocation();
		PC->Multicast_PlaySkillSoundAtLocation(SkillThreeSound, CurrentLocation);
	}

	AccumulatedDistance -= Increments * MovementDistanceInterval;
}

void UBunnySkillThreeAbility::PerformMovementTriggeredAttack()
{
	AActor* OwnerActor = GetAvatarActorFromActorInfo();
	UAbilitySystemComponent* ASC = GetFDAbilitySystemComponentFromActorInfo();
	const UPlayerAttributeSet* PlayerAttrSet = ASC->GetSet<UPlayerAttributeSet>();
	const float BunnySkillThreeRadius = TargetCapsuleSweepRadius * PlayerAttrSet->GetSkillRadius();


	// 현재 위치를 시작\(&목표\) 위치로 명시적으로 지정
	FGameplayAbilityTargetDataHandle CurrentTargetData;
	{
		FGameplayAbilityTargetData_LocationInfo* LocData = new FGameplayAbilityTargetData_LocationInfo();

		// 스윕 시작점: 소스 = 현재 소유 액터 트랜스폼
		LocData->SourceLocation.LocationType = EGameplayAbilityTargetingLocationType::ActorTransform;
		LocData->SourceLocation.SourceActor = OwnerActor;

		// 스윕 목표점(중심): 현재 액터 트랜스폼
		LocData->TargetLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
		LocData->TargetLocation.LiteralTransform = OwnerActor->GetActorTransform();

		CurrentTargetData.Add(LocData);
	}
	
	TArray<FHitResult> TargetHitResults = GetHitResultFromCapsuleSweepLocationTargetData(CurrentTargetData, TargetCapsuleHalfHeight, BunnySkillThreeRadius, TargetAmount, ShouldDebugDraw(), ETeamAttitude::Hostile, false);

	if (K2_HasAuthority())
	{
		for (FHitResult HitResult : TargetHitResults)
		{
			HitResult.bBlockingHit = false;
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitResult.GetActor()->GetName());

			FVector StartLocation = OwnerActor->GetActorLocation();
			FVector EndLocation = HitResult.ImpactPoint;
			
			float BunnySkillThreeDamage = (PlayerAttrSet->GetSkillDamage() * SkillDamageMultiplier) + (((PlayerAttrSet->GetSkillDamage() * 0.15f ) * (static_cast<int>(PlayerAttrSet->GetSpecialResource()) / 30)));
			bool bIsCriticalHit = FMath::FRand() <= PlayerAttrSet->GetSkillCriticalChance();
			
			if (bIsCriticalHit)
			{
				BunnySkillThreeDamage *= PlayerAttrSet->GetSkillCriticalDamage();
			}
			UE_LOG(LogTemp, Warning, TEXT("BunnySkillThreeDamage: %f"), BunnySkillThreeDamage);
			UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), BunnySkillThreeDamage, (EndLocation - StartLocation).GetSafeNormal(), HitResult, OwnerActor->GetInstigatorController(), OwnerActor, UDamageType::StaticClass());
		}
	}
}

void UBunnySkillThreeAbility::SpawnSplineAtLocation()
{
	const UPlayerAttributeSet* PlayerAttrSet = GetFDAbilitySystemComponentFromActorInfo()->GetSet<UPlayerAttributeSet>();
	if (!PlayerAttrSet) return;

	const float FinalScale = SpawnSplineScale * PlayerAttrSet->GetSkillRadius();

	UWorld* World = GetWorld();
	if (!World) return;

	AActor* OwnerActor = Cast<AActor>(GetAvatarActorFromActorInfo());
	if (!OwnerActor) return;

	const FVector SpawnLocation = OwnerActor->GetActorLocation();
	const FRotator SpawnRotation = OwnerActor->GetActorRotation();

	// 권한이 있으면 바로 서버에서 스폰 (서버 또는 Authority)
	if (K2_HasAuthority())
	{
		// 서버에서 직접 스폰
		if (!SpawnSplineClass) return;

		FActorSpawnParameters Params;
		Params.Owner = OwnerActor;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AActor* SpawnedActor = World->SpawnActor<AActor>(SpawnSplineClass, SpawnLocation, SpawnRotation, Params);
		if (SpawnedActor)
		{
			SpawnedActor->SetReplicates(true);
			SpawnedActor->SetReplicateMovement(true);
			SpawnedActor->SetActorScale3D(FVector(FinalScale));
			SpawnedActor->SetLifeSpan(1.0f);
		}
	}
	else
	{
		// 클라이언트면 서버에 스폰 요청
		Server_SpawnSplineAtLocation(SpawnLocation, SpawnRotation, FinalScale);
	}
}

void UBunnySkillThreeAbility::Server_SpawnSplineAtLocation_Implementation(const FVector& Location,
	const FRotator& Rotation, float Scale)
{
	if (!K2_HasAuthority()) return;

	UWorld* World = GetWorld();
	if (!World) return;
	if (!SpawnSplineClass) return;

	AActor* OwnerActor = Cast<AActor>(GetAvatarActorFromActorInfo());

	FActorSpawnParameters Params;
	Params.Owner = OwnerActor;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* SpawnedActor = World->SpawnActor<AActor>(SpawnSplineClass, Location, Rotation, Params);
	if (SpawnedActor)
	{
		SpawnedActor->SetReplicates(true);
		SpawnedActor->SetReplicateMovement(true);
		SpawnedActor->SetActorScale3D(FVector(Scale));
		SpawnedActor->SetLifeSpan(1.0f);
	}
}


void UBunnySkillThreeAbility::OnAbilityDurationEnd()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
