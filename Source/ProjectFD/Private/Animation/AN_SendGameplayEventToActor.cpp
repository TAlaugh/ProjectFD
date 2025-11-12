// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AN_SendGameplayEventToActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagsManager.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "GameFramework/PlayerState.h"


void UAN_SendGameplayEventToActor::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                          const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
		return;

	// 1. Owner에서 ASC 찾기
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);

	// 2. 없으면 PlayerState에서 ASC 찾기
	if (!ASC)
	{
		if (AFDPlayerCharacter* Character = Cast<AFDPlayerCharacter>(Owner))
		{
			if (APlayerState* PS = Character->GetPlayerState())
			{
				ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PS);
			}
		}
	}

	if (!ASC)
		return;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(ASC->GetOwner(), EventTag, FGameplayEventData());
}

FString UAN_SendGameplayEventToActor::GetNotifyName_Implementation() const
{
	if (EventTag.IsValid())
	{
		TArray<FName> TagNames;
		UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag, TagNames);
		if (TagNames.Num() > 0)
		{
			return TagNames.Last().ToString();
		}
	}
	return "None";
}
