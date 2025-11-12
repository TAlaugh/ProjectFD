// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/ANS_SendGameEventWithWeaponCollisionLocation.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "Components/Combat/Player/PlayerCombatComponent.h"
#include "Item/Weapon/WeaponBase.h"


void UANS_SendGameEventWithWeaponCollisionLocation::NotifyBegin(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp)
		return;
	if (!MeshComp->GetOwner() || !UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner()))
		return;
	if (EquipType == EWeaponEquipType::Equipped)
	{
		Weapon = Cast<AFDPlayerCharacter>( MeshComp->GetOwner())->GetCombatComponent()->GetEquippedWeapon(WeaponType);
	}
}

void UANS_SendGameEventWithWeaponCollisionLocation::NotifyTick(USkeletalMeshComponent* MeshComp,
															   UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!MeshComp)
		return;
	if (!MeshComp->GetOwner() || !UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner()))
		return;

	FVector StartLocation;
	FVector EndLocation;

	if (EquipType == EWeaponEquipType::Equipped)
	{
		StartLocation = Weapon->WeaponMesh->GetSocketLocation(FName("StartSocketName"));
		EndLocation = Weapon->WeaponMesh->GetSocketLocation(FName("EndSocketName"));
	}
	else
	{
		StartLocation = MeshComp->GetSocketLocation(FName("StartSocketName"));
		EndLocation = MeshComp->GetSocketLocation(FName("EndSocketName"));
	}
	
	FWeaponCollisionLocation WeaponCollisionLocation;
	WeaponCollisionLocation.StartLocation = StartLocation;
	WeaponCollisionLocation.EndLocation = EndLocation;
	WeaponCollisionLocations.Add(WeaponCollisionLocation);
}

void UANS_SendGameEventWithWeaponCollisionLocation::NotifyEnd(USkeletalMeshComponent* MeshComp,
                                                              UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp)
		return;
	if (!MeshComp->GetOwner() || !UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner()))
		return;

	FGameplayEventData Data;

	for (FWeaponCollisionLocation WeaponCollisionLocation : WeaponCollisionLocations)
	{
		FGameplayAbilityTargetData_LocationInfo* LocationInfo = new FGameplayAbilityTargetData_LocationInfo();
		LocationInfo->SourceLocation.LiteralTransform.SetLocation(WeaponCollisionLocation.StartLocation);
		LocationInfo->TargetLocation.LiteralTransform.SetLocation(WeaponCollisionLocation.EndLocation);
		Data.TargetData.Add(LocationInfo);
	}
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, Data);
	WeaponCollisionLocations.Reset();
}
