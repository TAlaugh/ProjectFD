// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AnimNotify/AN_BerserkNS.h"

#include "Character/Monster/Boss/FDBossCharacter.h"


void UAN_BerserkNS::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp)
	{
		return;
	}

	AActor* OwnerActor = MeshComp->GetOwner();

	if (!OwnerActor)
	{
		return;
	}
	
	AFDBossCharacter* Boss = Cast<AFDBossCharacter>(OwnerActor);

	if (!Boss)
	{
		return;
	}
	
	if (bActive)
	{
		Boss->NetMulticast_OnBerserk_NS();
	}
	if (!bActive)
	{
		Boss->NetMulticast_OffBerserk_NS();
	}
}
