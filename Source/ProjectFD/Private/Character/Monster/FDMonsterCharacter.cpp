// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Monster/FDMonsterCharacter.h"

#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "Components/Combat/Monster/EnemyCombatComponent.h"
#include "DataAssets/StartUpData/Monster/FDDataAsset_EnemyStartUpData.h"
#include "Engine/AssetManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/Monster/EnemyAbilitySystemComponent.h"
#include "GAS/Monster/EnemyAttributeSet.h"
#include "MotionWarpingComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/Monster/Boss/FDBossCharacter.h"
#include "Character/Monster/MIN/FDMINCharacter.h"
#include "Components/BoxComponent.h"
#include "Controller/FDPlayerController.h"
#include "Engine/DamageEvents.h"
#include "FunctionLibrary/FDFunctionLibrary.h"
#include "Gameframework/FDGameMode.h"
#include "Gameframework/FDPlayerState.h"
#include "GAS/FDGameplayStatic.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Widget/HUD/BossStatusHUD.h"
#include "Widget/HUD/Damage/DamageWidgetActor.h"


AFDMonsterCharacter::AFDMonsterCharacter()
{
	bReplicates = true;
	SetReplicates(true);
	SetReplicateMovement(true);

	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	GetMesh()->bNoSkeletonUpdate = false;

	GetMesh()->SetIsReplicated(true);
	GetCharacterMovement()->SetIsReplicated(true);
	GetCapsuleComponent()->SetIsReplicated(true);
	
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 180.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1000.f;

	GetMesh()->bReceivesDecals = false;

	EnemyCombatComponent = CreateDefaultSubobject<UEnemyCombatComponent>("EnemyCombatComponent");
	ASC = CreateDefaultSubobject<UEnemyAbilitySystemComponent>("EnemyAbilitySystemComponent");
	EnemyAttributeSet = CreateDefaultSubobject<UEnemyAttributeSet>("EnemyAttributeSet");
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>("MotionWarpingComponent");
}

UAbilitySystemComponent* AFDMonsterCharacter::GetAbilitySystemComponent() const
{
	return GetEnemyAbilitySystemComponent();
}

UPawnCombatComponent* AFDMonsterCharacter::GetPawnCombatComponent() const
{
	return EnemyCombatComponent;
}

void AFDMonsterCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFDMonsterCharacter, TargetActor);
}

void AFDMonsterCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	InitEnemyStartUpData();
	
	// Base
	if (ASC)
	{
		ASC->InitAbilityActorInfo(this, this);
		
		ensureMsgf(!EnemyStartUpData.IsNull(), TEXT("Forgot to assign start up data to %s"), *GetName());
	}
	
}

void AFDMonsterCharacter::InitEnemyStartUpData()
{
	if (EnemyStartUpData.IsNull())
	{
		return;
	}

	UAssetManager::GetStreamableManager().RequestAsyncLoad(
		EnemyStartUpData.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda(
			[this]()
			{
				if (UFDDataAsset_EnemyStartUpData* LoadedData = EnemyStartUpData.Get())
				{
					LoadedData->GiveToAbilitySystemComponent(Cast<UFDAbilitySystemComponent>(ASC));
				}
			}
		)
	);
}

void AFDMonsterCharacter::OnRep_TargetActor()
{
	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
		{
			BB->SetValueAsObject("TargetActor", TargetActor);
		}
	}
}

void AFDMonsterCharacter::UpdateNearestTarget()
{
	if (!HasAuthority())
	{
		return;
	}

	AActor* Nearest = nullptr;
	float MinDist = FLT_MAX;
	
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AFDPlayerController* PC = Cast<AFDPlayerController>(It->Get()))
		{
			if (APawn* Pawn = PC->GetPawn())
			{
				if (AFDPlayerState* PS = Pawn->GetPlayerState<AFDPlayerState>())
				{
					if (IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(PS))
					{
						if (TeamAgent->GetGenericTeamId() != FGenericTeamId(0)) continue;
					}
				}
				
				float Dist = FVector::Dist(Pawn->GetActorLocation(), GetActorLocation());
				if (Dist < MinDist)
				{
					MinDist = Dist;
					Nearest = Pawn;
				}
			}
		}
	}
	
	if (Nearest && Nearest != TargetActor)
	{
		TargetActor = Nearest;
		OnRep_TargetActor(); // 서버에서도 Blackboard 갱신
	}
}

float AFDMonsterCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
		AActor* HitActor = PointDamageEvent->HitInfo.GetActor();
		FName HitBone = PointDamageEvent->HitInfo.BoneName;
		FVector HitLocation = PointDamageEvent->HitInfo.ImpactPoint;

		AFDBossCharacter* BC = Cast<AFDBossCharacter>(HitActor);
		if (BC)
		{
			if (UFDFunctionLibrary::NativeDoesActorHaveTag(BC, UFDGameplayStatic::GetBossSpawn()))
			{
				return FinalDamage;
			}
		}
		
		// true 펀치 / false 총
		bIsPunch = PointDamageEvent->HitInfo.bBlockingHit;
		
		bool bBlockBodyDamage = ProcessBossPartDamage(HitActor, FinalDamage, HitBone);
		
		if (!bBlockBodyDamage && !bIsPunch)
		{
			// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, EventInstigator->GetPawn()->GetActorNameOrLabel());
			
			HandleShieldAndHealth(FinalDamage, HitActor);

			if (BC)
			{
				Multicast_UpdateBossHUD(BC, BC->GetEnemyAttributeSet()->GetHealth(), BC->GetEnemyAttributeSet()->GetMaxHealth(),
					BC->GetEnemyAttributeSet()->GetShield(), BC->GetEnemyAttributeSet()->GetMaxShield());
			}
			
			if (EventInstigator)
			{
				Multicast_ShowDamageUI(FinalDamage, HitLocation, EventInstigator);
			}
		}
		if (bBlockBodyDamage && !bIsPunch && HitBone != "Bn_CorePlasma")
		{
			FinalDamage = 0;
			if (EventInstigator)
			{
				Multicast_ShowDamageUI(FinalDamage, HitLocation, EventInstigator);
			}
		}
	}

	return FinalDamage;
}

void AFDMonsterCharacter::HandleShieldAndHealth(float Damage, AActor* HitActor)
{
	if (GetEnemyAttributeSet()->GetShield() > 0.f)
	{
		if (GetEnemyAttributeSet()->GetShield() < Damage)
		{
			float RemainingDamage = Damage - GetEnemyAttributeSet()->GetShield();
			GetEnemyAttributeSet()->SetShield(0);
			GetEnemyAttributeSet()->SetHealth(FMath::Max(GetEnemyAttributeSet()->GetHealth() - RemainingDamage, 0.f));
		}
		else
		{
			GetEnemyAttributeSet()->SetShield(GetEnemyAttributeSet()->GetShield() - Damage);
		}
	}
	else
	{
		GetEnemyAttributeSet()->SetHealth(FMath::Max(GetEnemyAttributeSet()->GetHealth() - Damage, 0.f));
	}

	if (GetEnemyAttributeSet()->GetHealth() <= 0.f)
	{
		GetEnemyAttributeSet()->SetHealth(0);
		DropItemOnDeath(HitActor);
	}
}

bool AFDMonsterCharacter::ProcessBossPartDamage(AActor* HitActor, float Damage, FName HitBone)
{
	AFDBossCharacter* Boss = Cast<AFDBossCharacter>(HitActor);
    if (!Boss)
    {
    	return false; // 일반 몬스터 → 본체 데미지 처리
    }

    const bool bIsBerserkMode = UFDFunctionLibrary::NativeDoesActorHaveTag(this, UFDGameplayStatic::GetBerserkModeTag());

    for (FBossPart& Part : Boss->BossPartsInfo)
    {
        if (Part.BoneName != HitBone)
        {
            continue;
        }
    	
        if (Part.bOnlyDamageInBerserk && bIsBerserkMode)
        {
            // Berserk 전용 파츠 -> 데미지 O, 본체 X
            Part.PartHealth -= Damage;

        	if (Part.PartHealth > Part.MaxPartHealth * 0.4)
        	{
        		Boss->NetMulticast_TwinklePart(Part.BoneName);
        	}
        	else if (Part.PartHealth <= Part.MaxPartHealth * 0.4)
        	{
        		Boss->NetMulticast_TwinklePart_Warning(Part.BoneName);
        	}
        	
            if (Part.PartHealth <= 0.f)
            {
                Part.PartHealth = Part.MaxPartHealth; // 파괴는 안 하고 기믹용
                if (!UFDFunctionLibrary::NativeDoesActorHaveTag(this, Part.DestroyTag))
                {
                    UFDFunctionLibrary::AddGameplayTagToActorIfNone(this, Part.DestroyTag);
                    UFDFunctionLibrary::AddGameplayTagToActorIfNone(this, UFDGameplayStatic::PlayDestroyAM());
                    if (UEnemyAbilitySystemComponent* BossASC = Cast<UEnemyAbilitySystemComponent>(Boss->GetEnemyAbilitySystemComponent()))
                    {
                        BossASC->TryActivateAbilityByTag(Part.DestroyTag);
                    }
                }
            }
            return true; // 본체 데미지 차단
        }

        if (Part.bOnlyDamageInBerserk && !bIsBerserkMode)
        {
            // Berserk 전용 파츠 -> 데미지 X, 본체 X
            return false;
        }

        if (!Part.bOnlyDamageInBerserk && !bIsBerserkMode)
        {
            // 일반 파츠 -> 파츠 O, 본체 O
            if (bIsPunch)
            {
	            Part.PartHealth -= Part.MaxPartHealth * 0.1;
            }
        	if (!bIsPunch)
        	{
				Part.PartHealth -= Damage;
        	}

            if (Part.PartHealth > Part.MaxPartHealth * 0.4)
            {
				Boss->NetMulticast_TwinklePart(Part.BoneName);
            }
        	else if (Part.PartHealth <= Part.MaxPartHealth * 0.4)
        	{
        		Boss->NetMulticast_TwinklePart_Warning(Part.BoneName);
        	}
        	
            if (Part.BoneName == "Dm_Clav_L")
            {
	            if (!Part.bCanAttach && Part.PartHealth <= (Part.MaxPartHealth * 0.4) && !UFDFunctionLibrary::NativeDoesActorHaveTag(Boss, UFDGameplayStatic::GetDestroyClav_L()))
	            {
	            	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "CanAttach Clav_L");
		            Part.bCanAttach = true;
	            }
            }
        	if (Part.BoneName == "Dm_Clav_R")
        	{
        		if (!Part.bCanAttach && Part.PartHealth <= (Part.MaxPartHealth * 0.4) && !UFDFunctionLibrary::NativeDoesActorHaveTag(Boss, UFDGameplayStatic::GetDestroyClav_R()))
        		{
	            	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "CanAttach Clav_R");
        			Part.bCanAttach = true;
        		}
        	}
        	
            if (Part.PartHealth <= 0.f)
            {
                Part.PartHealth = 0.f;
                Boss->DestroyPart(Part.BoneName);
                if (Part.bCanAttach)
                {
	                Part.bCanAttach = false;
                }
                if (!UFDFunctionLibrary::NativeDoesActorHaveTag(this, Part.DestroyTag))
                {
                    UFDFunctionLibrary::AddGameplayTagToActorIfNone(this, Part.DestroyTag);
                    UFDFunctionLibrary::AddGameplayTagToActorIfNone(this, UFDGameplayStatic::PlayDestroyAM());
                    if (UEnemyAbilitySystemComponent* BossASC = Cast<UEnemyAbilitySystemComponent>(Boss->GetEnemyAbilitySystemComponent()))
                    {
                        BossASC->TryActivateAbilityByTag(Part.DestroyTag);
                    }
                }
            }
            return false;
        }

        if (!Part.bOnlyDamageInBerserk && bIsBerserkMode)
        {
        	// 일반 파츠 -> 파츠 X, 본체 O
            return true;
        }
    }
	
    return bIsBerserkMode; // Berserk 상태 -> 본체 차단, 일반 상태 -> 본체 허용
}

void AFDMonsterCharacter::SpawnWidget(float Damage, FVector SpawnLocation)
{
	if (HasAuthority())
	{
		return;
	}
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ADamageWidgetActor* DamageActor = GetWorld()->SpawnActor<ADamageWidgetActor>(
		DamageTextActorClass,
		SpawnLocation,
		FRotator::ZeroRotator,
		SpawnParams
	);
	
	if (DamageActor)
	{
		DamageActor->InitDamage(Damage);
	}
}

void AFDMonsterCharacter::DropItemOnDeath(AActor* HitActor)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!HitActor)
	{
		return;
	}
	
	Multicast_MonsterDead(HitActor);
	
	if (AFDBossCharacter* Boss = Cast<AFDBossCharacter>(HitActor))
	{
		Boss->DropBossItems();
		// Boss->NetMulticast_HideBossHUD();
		if (AFDGameMode* GameMode = GetWorld()->GetAuthGameMode<AFDGameMode>())
		{
			GameMode->CallBossDead();
		}

		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (AFDPlayerController* FDPC = Cast<AFDPlayerController>(It->Get()))
			{
				FDPC->Client_SuccessAnim();
			}
		}
		
		TArray<AActor*> MINs;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFDMINCharacter::StaticClass(), MINs);

		for (AActor* Actor : MINs)
		{
			if (AFDMINCharacter* MIN = Cast<AFDMINCharacter>(Actor))
			{
				MIN->Destroy();
			}
		}
	}
	
	if (AFDMINCharacter* MIN = Cast<AFDMINCharacter>(HitActor))
	{
		MIN->DropMonsterItem();
	}
	
	UFDFunctionLibrary::AddGameplayTagToActorIfNone(this, UFDGameplayStatic::GetMonsterDeadTag());
}

void AFDMonsterCharacter::Multicast_UpdateBossHUD_Implementation(AActor* HitActor, float CurrentHp, float MaxHp,
	float CurrentShield, float MaxShield)
{
	if (AFDBossCharacter* Boss = Cast<AFDBossCharacter>(HitActor))
	{
		Boss->OnBossStatChanged.Broadcast(CurrentHp, MaxHp, CurrentShield, MaxShield);
	}
}

void AFDMonsterCharacter::Multicast_MonsterDead_Implementation(AActor* HitActor)
{
	if (AFDBossCharacter* Boss = Cast<AFDBossCharacter>(HitActor))
	{
		Boss->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);	
		Boss->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (AFDMINCharacter* MIN = Cast<AFDMINCharacter>(HitActor))
	{
		MIN->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MIN->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MIN->GetMeleeCollision()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AFDMonsterCharacter::Multicast_ShowDamageUI_Implementation(float Damage, FVector HitLocation,
                                                                AController* InstigatorController)
{
	APlayerController* LocalPC = GetWorld()->GetFirstPlayerController();
	
	APawn* LocalPawn = LocalPC ? LocalPC->GetPawn() : nullptr;
	
	APawn* InstigatorPawn = InstigatorController ? InstigatorController->GetPawn() : nullptr;
	
	if (LocalPawn == InstigatorPawn)
	{
		FVector RandomOffset(
			FMath::FRandRange(-100.f, 100.f),
			FMath::FRandRange(-100.f, 100.f),
			FMath::FRandRange(0.f, 50.f)
		);
		FVector FinalLocation = HitLocation + RandomOffset;
		
		SpawnWidget(Damage, FinalLocation);
	}
}
