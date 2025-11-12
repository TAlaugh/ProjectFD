// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AnimNoitfyState/ANS_PlayerHangOnPart.h"

#include "Character/Player/FDPlayerCharacter.h"
#include "FunctionLibrary/FDFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"


void UANS_PlayerHangOnPart::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration)
{
	if (!MeshComp) return;
	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor) return;

	if (!OwnerActor->HasAuthority()) return;

	Players.Reset();
	
	UWorld* World = MeshComp->GetWorld();
	UGameplayStatics::GetAllActorsOfClass(World, AFDPlayerCharacter::StaticClass(), Players);
}

void UANS_PlayerHangOnPart::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	if (!MeshComp) return;

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor) return;

	if (!OwnerActor->HasAuthority()) return;
	
	for (AActor* PlayerActor : Players)
	{
		if (UFDFunctionLibrary::NativeDoesActorHaveTag(PlayerActor, UFDGameplayStatic::GetOwnerHangOnTag()))
		{
			UFDFunctionLibrary::AddGameplayTagToActorIfNone(PlayerActor, UFDGameplayStatic::BodySwing());
		}
	}
}

void UANS_PlayerHangOnPart::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;
	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor) return;

	if (!OwnerActor->HasAuthority()) return;
	
	for (AActor* PlayerActor : Players)
	{
		if (UFDFunctionLibrary::NativeDoesActorHaveTag(PlayerActor, UFDGameplayStatic::BodySwing()))
		{
			UFDFunctionLibrary::RemoveGameplayFromActorIfFound(PlayerActor, UFDGameplayStatic::BodySwing());
		}
	}
}
