// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AN/AN_SendTargetGroup.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameplayEffectTypes.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

void UAN_SendTargetGroup::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
		return;

	if (TargetSocketNames.Num() <= 1)
		return;

	if (!MeshComp->GetOwner() || !UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner()))
	{
		return;
	}

	FGameplayEventData Data;
	TSet<AActor*> HitActors;
	AActor* OwnerActor = MeshComp->GetOwner();
	const IGenericTeamAgentInterface* OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(OwnerActor);
	// 이미 트레이스된 액터를 저장하기 위한 집합
	TSet<AActor*> TracedActors;
	
	for (int i = 1; i < TargetSocketNames.Num(); ++i)
	{
		FVector StartLoc = MeshComp->GetSocketLocation(TargetSocketNames[i-1]);
		FVector EndLoc = MeshComp->GetSocketLocation(TargetSocketNames[i]);

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
		TArray<FHitResult> HitResults;

		TArray<AActor*> ActorsToIgnore;
		if (bIgnoreOwner)
		{
			ActorsToIgnore.Add(OwnerActor);
		}

		EDrawDebugTrace::Type DrawDebugTrace = bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

		UKismetSystemLibrary::SphereTraceMultiForObjects(MeshComp, StartLoc, EndLoc, SphereSweepRadius, 
			ObjectTypes, false, ActorsToIgnore, DrawDebugTrace, HitResults, false);

		
		for (const FHitResult& HitResult : HitResults)
		{
			AActor* HitActor = HitResult.GetActor();
			if (!HitActor) continue;

			// 이미 트레이스된 액터인지 확인
			if (TracedActors.Contains(HitActor))
			{
				continue; // 이미 트레이스된 액터면 다음 HitResult로 넘어감
			}
			
			if (HitActors.Contains(HitActor))
			{
				continue;
			}

			if (OwnerTeamInterface)
			{
				if (OwnerTeamInterface->GetTeamAttitudeTowards(*HitActor) != TargetTeam)
				{
					continue;
				}
			}

			FVector ImpactPoint = HitResult.ImpactPoint;
			if (bDrawDebug)
			{
				DrawDebugLine(MeshComp->GetWorld(), StartLoc, ImpactPoint, FColor::Green, false, 1, 0, 1);
			}
			
			// if (LightingEffect)
			// {
			// 	UNiagaraFunctionLibrary::SpawnSystemAtLocation(MeshComp->GetWorld(), LightingEffect, ImpactPoint, FRotator::ZeroRotator, FVector::OneVector, true, true, ENCPoolMethod::None, true);
			// }
			
			FGameplayAbilityTargetData_SingleTargetHit* TargetHit = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
			Data.TargetData.Add(TargetHit);
			SendLocalGameplayCue(HitResult);

			HitActors.Add(HitActor);
			TracedActors.Add(HitActor); // 트레이스된 액터 목록에 추가
		}
	}

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, Data);
}

void UAN_SendTargetGroup::SendLocalGameplayCue(const FHitResult& HitResult) const
{
	FGameplayCueParameters CueParam;
	CueParam.Location = HitResult.ImpactPoint;
	CueParam.Normal = HitResult.ImpactNormal;

	for (const FGameplayTag& GameplayCueTag : TriggerGameplayCueTags)
	{
		UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(HitResult.GetActor(), GameplayCueTag, EGameplayCueEvent::Executed, CueParam);
	}
}
