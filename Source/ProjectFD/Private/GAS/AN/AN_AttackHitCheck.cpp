// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AN/AN_AttackHitCheck.h"

#include "AbilitySystemBlueprintLibrary.h"


UAN_AttackHitCheck::UAN_AttackHitCheck()
{
}

FString UAN_AttackHitCheck::GetNotifyName_Implementation() const
{
	return TEXT("AttackHitCheck");
}

void UAN_AttackHitCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
// {
// 	Super::Notify(MeshComp, Animation, EventReference);
// 	
// 	if (MeshComp)
// 	{
// 		if (AActor* OwnerActor = MeshComp->GetOwner()->GetOwner())
// 		{
// 			if(!OwnerActor)
// 			{
// 				OwnerActor = MeshComp->GetOwner();
// 			}
// 			
// 			if (OwnerActor)
// 			{
// 				UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerActor);
// 				if (ASC)
// 				{
// 					FGameplayEventData PayLoadData;
// 					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, TriggerGameplayTag, PayLoadData);
// 				}
// 			}
// 		}
// 	}
// }

{
	Super::Notify(MeshComp, Animation, EventReference);
	if (!MeshComp)
	{
		return;
	}

	AActor* RawOwner = MeshComp->GetOwner();
	AActor* Candidates[3] = {
		RawOwner,
		RawOwner ? RawOwner->GetOwner() : nullptr,      // 무기 Actor일 경우 소유 캐릭터
		RawOwner ? RawOwner->GetInstigator() : nullptr  // 안전망
	};

	AActor* AbilityOwner = nullptr;
	UAbilitySystemComponent* ASC = nullptr;

	for (AActor* Candidate : Candidates)
	{
		if (!Candidate)
		{
			continue;
		}
		ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Candidate);
		if (ASC)
		{
			AbilityOwner = Candidate;
			break;
		}
	}

	if (!AbilityOwner || !ASC || !TriggerGameplayTag.IsValid())
	{
		return;
	}

	bool bShouldSend = AbilityOwner->HasAuthority();
	if (!bShouldSend)
	{
		if (const APawn* Pawn = Cast<APawn>(AbilityOwner))
		{
			bShouldSend = Pawn->IsLocallyControlled();
		}
	}
	if (!bShouldSend)
	{
		return;
	}

	FGameplayEventData Payload;
	Payload.EventTag = TriggerGameplayTag;
	Payload.Instigator = AbilityOwner;
	Payload.OptionalObject = MeshComp;
	Payload.Target = AbilityOwner;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(AbilityOwner, TriggerGameplayTag, Payload);
}