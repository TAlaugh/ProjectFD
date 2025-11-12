// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AnimNotify/AN_ExecutionCallLoopCount.h"

#include "Character/Monster/Boss/FDBossCharacter.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "Controller/FDPlayerController.h"
#include "GameFramework/Character.h"
#include "Item/Projectile/FDBossECSpawn.h"
#include "Kismet/GameplayStatics.h"


void UAN_ExecutionCallLoopCount::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ACharacter* OwnerChar = Cast<ACharacter>(MeshComp->GetOwner());
	if (!OwnerChar) return;

	if (!OwnerChar || !OwnerChar->HasAuthority()) return;

	if (!GetWorld())
	{
		return;
	}
	
	if (AFDBossCharacter* Boss = Cast<AFDBossCharacter>(OwnerChar))
	{
		Boss->ExecutionCall_LoopCount++;
		
		if (Boss->ExecutionCall_LoopCount == 1)
		{
			for (FConstPlayerControllerIterator It = Boss->GetWorld()->GetPlayerControllerIterator(); It; ++It)
			{
				if (AFDPlayerController* PC = Cast<AFDPlayerController>(It->Get()))
				{
					PC->Client_FadeIn();
				}
			}
		}
		
		if (Boss->ExecutionCall_LoopCount == TeleportPlayerCount)
		{
			TeleportPlayer(Boss);
			Boss->ExecutionCall_DamageVector = Boss->GetActorLocation();
			
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AFDBossECSpawn* WarningEffect = Boss->GetWorld()->SpawnActor<AFDBossECSpawn>(
					WarningEffectActorClass,
					Boss->ExecutionCall_DamageVector - FVector(0.f,0.f,87.f),
					FRotator::ZeroRotator,
					SpawnParameters
				);

			for (FConstPlayerControllerIterator It = Boss->GetWorld()->GetPlayerControllerIterator(); It; ++It)
			{
				if (AFDPlayerController* PC = Cast<AFDPlayerController>(It->Get()))
				{
					PC->Client_FadeOut();
				}
			}
		}
		if (Boss->ExecutionCall_LoopCount >= AM_EndSectionCount)
		{
			if (USkeletalMeshComponent* Mesh = Boss->GetMesh())
			{
				if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
				{
					// 현재 재생 중인 Montage 가져오기
					UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage();
					if (CurrentMontage)
					{
						// End 섹션으로 점프
						AnimInstance->Montage_JumpToSection(FName("End"), CurrentMontage);
						Boss->ExecutionCall_LoopCount = 0;
					}
				}
			}
		}
	}
}

void UAN_ExecutionCallLoopCount::TeleportPlayer(AFDBossCharacter* Boss)
{
	UWorld* World = Boss->GetWorld();
	TArray<AActor*> Players;
	UGameplayStatics::GetAllActorsOfClass(World, AFDPlayerCharacter::StaticClass(), Players);

	for (AActor* PlayerActor : Players)
	{
		if (AFDPlayerCharacter* Player = Cast<AFDPlayerCharacter>(PlayerActor))
		{
			FVector BossLocation = Boss->GetActorLocation();
			Player->SetActorLocation(BossLocation + FVector(0.f, 0.f, 1000.f));
		}
	}
}