// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AnimNotify/AN_StatusDead.h"

#include "AIController.h"
#include "Character/Monster/FDMonsterCharacter.h"
#include "Character/Monster/Boss/FDBossCharacter.h"
#include "Character/Monster/MIN/FDMINCharacter.h"


void UAN_StatusDead::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp)
	{
		return;
	}
	
	ACharacter* OwnerChar = Cast<ACharacter>(MeshComp->GetOwner());
	if (!OwnerChar) return;

	if (!OwnerChar->HasAuthority())
	{
		return;
	}

	AAIController* AIC = Cast<AAIController>(OwnerChar->GetInstigatorController());
	AFDMonsterCharacter* Monster = Cast<AFDMonsterCharacter>(OwnerChar);

	if (AIC)
	{
		// AIC->StopMovement();
	}
	
	if (Monster)
	{
		if (AFDBossCharacter* Boss = Cast<AFDBossCharacter>(Monster))
		{
			for (auto& Part : Boss->Parts)
			{
				if (Part)
				{
					Part->DestroyComponent();
				}
			}
		}
		
		if (AFDMINCharacter* MIN = Cast<AFDMINCharacter>(Monster))
		{
			// 
		}
	}
}

