// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/TA/TA_Trace.h"

#include "Abilities/GameplayAbility.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"


ATA_Trace::ATA_Trace()
{
}

void ATA_Trace::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
}

void ATA_Trace::ConfirmTargetingAndContinue()
{
	//Super::ConfirmTargetingAndContinue();

	if (SourceActor)
	{
		FGameplayAbilityTargetDataHandle DataHandle = MakeTargetData();
		TargetDataReadyDelegate.Broadcast(DataHandle);
	}
}

FGameplayAbilityTargetDataHandle ATA_Trace::MakeTargetData() const
{
	ACharacter* Character = Cast<ACharacter>(SourceActor);

	FHitResult OutHitResult;
	const float AttackRange = 200.0f;
	const float AttackRadius = 50.0f;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(UTA_Trace), false, Character);
	const FVector Forward = Character->GetActorForwardVector();
	const FVector Start = Character->GetActorLocation() + Forward * Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + Forward * AttackRange;

	bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(AttackRadius), Params);

	FGameplayAbilityTargetDataHandle DataHandle;
	if (HitDetected)
	{
		//DataHandle.Add(new FGameplayAbilityTargetData_SingleTargetHit(OutHitResult));
		FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(OutHitResult);
		DataHandle.Add(TargetData);
	}


#if ENABLE_DRAW_DEBUG
	if (bShowDebug)
	{
		FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
		float CapsuleHalfHeight = AttackRange * 0.5f;
		FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;
		DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(Forward).ToQuat(), DrawColor, false, 2.0f);
	}
#endif
	return DataHandle;
}
