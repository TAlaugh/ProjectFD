// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AnimNotify/AN_RemoveTag.h"

#include "Character/Monster/Boss/FDBossCharacter.h"
#include "FunctionLibrary/FDFunctionLibrary.h"
#include "GameFramework/Character.h"


void UAN_RemoveTag::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	ACharacter* OwnerChar = Cast<ACharacter>(MeshComp->GetOwner());
	if (!OwnerChar) return;

	if (!OwnerChar->HasAuthority())
	{
		return;
	}
	
	UFDFunctionLibrary::RemoveGameplayFromActorIfFound(OwnerChar, RemoveTag);
	UFDFunctionLibrary::AddGameplayTagToActorIfNone(OwnerChar, GrantTag);

	if (CallEndBerserk)
	{
		if (AFDBossCharacter* Boss = Cast<AFDBossCharacter>(OwnerChar))
		{
			Boss->DestroyEndBerserk();
		}
	}
}
