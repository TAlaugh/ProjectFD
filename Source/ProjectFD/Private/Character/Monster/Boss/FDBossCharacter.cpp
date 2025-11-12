// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Monster/Boss/FDBossCharacter.h"

#include "NiagaraComponent.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Controller/FDPlayerController.h"
#include "DataAssets/DropItem/DropItemDataAsset.h"
#include "FunctionLibrary/FDFunctionLibrary.h"
#include "GAS/FDAbilitySystemComponent.h"
#include "GAS/Monster/EnemyAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PhysicsEngine/BodySetup.h"
#include "Widget/HUD/BossStatusHUD.h"

AFDBossCharacter::AFDBossCharacter()
{
	bReplicates = true;

	HeadShell_L = CreateDefaultSubobject<USkeletalMeshComponent>("HeadShell_L");
	HeadShell_L->SetupAttachment(GetMesh(), "Dm_HeadShell_L");
	HeadShell_L->SetIsReplicated(true);

	HeadShell_R = CreateDefaultSubobject<USkeletalMeshComponent>("HeadShell_R");
	HeadShell_R->SetupAttachment(GetMesh(), "Dm_HeadShell_R");
	HeadShell_R->SetIsReplicated(true);

	RealCore = CreateDefaultSubobject<USkeletalMeshComponent>("RealCore");
	RealCore->SetupAttachment(GetMesh(), "Dm_RealCore_Weakness");
	RealCore->SetIsReplicated(true);

	CoreShell = CreateDefaultSubobject<USkeletalMeshComponent>("CoreShell");
	CoreShell->SetupAttachment(GetMesh(), "Dm_RealCoreShell");
	CoreShell->SetIsReplicated(true);
	
	Clav_L = CreateDefaultSubobject<USkeletalMeshComponent>("Clav_L");
	Clav_L->SetupAttachment(GetMesh(), "Dm_Clav_L");
	Clav_L->SetIsReplicated(true);
	
	Clav_R = CreateDefaultSubobject<USkeletalMeshComponent>("Clav_R");
	Clav_R->SetupAttachment(GetMesh(), "Dm_Clav_R");
	Clav_R->SetIsReplicated(true);

	Knee_L = CreateDefaultSubobject<USkeletalMeshComponent>("Knee_L");
	Knee_L->SetupAttachment(GetMesh(), "Dm_Knee_L");
	Knee_L->SetIsReplicated(true);

	Knee_R = CreateDefaultSubobject<USkeletalMeshComponent>("Knee_R");
	Knee_R->SetupAttachment(GetMesh(), "Dm_Knee_R");
	Knee_R->SetIsReplicated(true);
	
	Plasma = CreateDefaultSubobject<USkeletalMeshComponent>("Plasma");
	Plasma->SetLeaderPoseComponent(GetMesh());
	Plasma->SetupAttachment(GetMesh());
	Plasma->SetIsReplicated(true);
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BoxCollision = CreateDefaultSubobject<UBoxComponent>("Guillotine_Collision");
	BoxCollision->SetupAttachment(GetMesh(), "Dm_Sword_R001");

	BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BerserkNS = CreateDefaultSubobject<UNiagaraComponent>("BerserkNS");
	BerserkNS->SetupAttachment(RootComponent);
}

void AFDBossCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(AFDBossCharacter, bIsBerserk, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AFDBossCharacter, BossPartsInfo, COND_None, REPNOTIFY_Always);
}

void AFDBossCharacter::DropBossItems()
{
	if (!HasAuthority()) return;
	if (!DropItemDataAsset) return;
	
	const TArray<FDropItemData>& Items = DropItemDataAsset->BossDrops;
	if (Items.Num() == 0) return;

	FVector TargetLocation = GetActorLocation();
	
	int32 DropCount = FMath::RandRange(6, 7);

	for (int32 i = 0; i < DropCount; ++i)
	{
		float Radius = 500.f;
		float AngleDeg = (360.f / DropCount) * i;
		float Radian = FMath::DegreesToRadians(AngleDeg);
		
		FVector Offset(FMath::Cos(Radian) * Radius, FMath::Sin(Radian) * Radius, 0.f);
		FVector SpawnPos = TargetLocation + Offset + FVector(0, 0, 100.f);
		
		FHitResult Hit;
		FVector TraceEnd = SpawnPos - FVector(0, 0, 1000.f);
		if (GetWorld()->LineTraceSingleByChannel(Hit, SpawnPos, TraceEnd, ECC_Visibility))
		{
			SpawnPos = Hit.Location + FVector(0, 0, 500.f);
		}
		else
		{
			SpawnPos = TargetLocation + FVector(0, 0, 500.f);
		}
		
		float TotalWeight = 0.f;
		for (const FDropItemData& Item : Items)
		{
			TotalWeight += Item.DropRate;
		}

		float RandomPoint = FMath::FRandRange(0.f, TotalWeight);
		float Accum = 0.f;

		for (const FDropItemData& Item : Items)
		{
			Accum += Item.DropRate;
			if (RandomPoint <= Accum && Item.DropItemClass)
			{
				FActorSpawnParameters Params;
				Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

				ADropItemBase* Drop = GetWorld()->SpawnActor<ADropItemBase>(Item.DropItemClass, SpawnPos, FRotator::ZeroRotator, Params);
				if (Drop)
				{
					Drop->SetReplicates(true);
				}
				
				break;
			}
		}
	}
}

void AFDBossCharacter::BeginPlay()
{
	Super::BeginPlay();

	Parts.AddUnique(HeadShell_L);
	Parts.AddUnique(HeadShell_R);
	Parts.AddUnique(RealCore);
	Parts.AddUnique(CoreShell);
	Parts.AddUnique(Knee_R);
	Parts.AddUnique(Knee_L);
	Parts.AddUnique(Clav_L);
	Parts.AddUnique(Clav_R);
	Parts.AddUnique(Plasma);
	
	BoxCollision->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionBeginOverlap);
	BoxCollision->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionEndOverlap);

	NetMulticast_OffBerserk_NS();
	
	if (HasAuthority())
	{
		IncreaseBerserkGage();
		UFDFunctionLibrary::AddGameplayTagToActorIfNone(this, UFDGameplayStatic::GetBossSpawn());
		UFDFunctionLibrary::AddGameplayTagToActorIfNone(this, UFDGameplayStatic::GetNormalModeTag());
	}

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, [this]()
	{
		if (AFDPlayerController* PC = Cast<AFDPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
		{
			PC->ShowBossStatusHUD(this);
		}
	}, 0.5f, false);
}

void AFDBossCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetEnemyAttributeSet() && GetEnemyAttributeSet()->GetBerserkGage() >= GetEnemyAttributeSet()->GetMaxBerserkGage() && !bIsBerserk)
	{
		StartBerserk();
	}

	if (UFDFunctionLibrary::NativeDoesActorHaveTag(this, UFDGameplayStatic::GetBossSpawn()))
	{
		GetEnemyAttributeSet()->SetBerserkGage(0.f);
	}

	if (!UFDFunctionLibrary::NativeDoesActorHaveTag(this, UFDGameplayStatic::PlayerAttachParts()) &&
		UFDFunctionLibrary::NativeDoesActorHaveTag(this, UFDGameplayStatic::BerserkGageBlock()))
	{
		UFDFunctionLibrary::RemoveGameplayFromActorIfFound(this, UFDGameplayStatic::BerserkGageBlock());
	}
}


void AFDBossCharacter::IncreaseBerserkGage()
{
	if (bIsBerserk)
	{
		return;
	}
	
	if (GetEnemyAbilitySystemComponent())
	{
		if (IncreaseBerserkGageGE)
		{
			FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(IncreaseBerserkGageGE, 1.f, ContextHandle);
			if (SpecHandle.IsValid())
			{
				ActiveIncreaseBerserkGage = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, "IncreaseBerserkGage");
			}
		}
	}
}

void AFDBossCharacter::StartBerserk()
{
	if (bIsBerserk || !HasAuthority())
	{
		return;
	}

	// 증가 GE 지우기
	if (ActiveIncreaseBerserkGage.IsValid())
	{
		if (ASC)
		{
			ASC->RemoveActiveGameplayEffect(ActiveIncreaseBerserkGage);
		}
		ActiveIncreaseBerserkGage.Invalidate();
	}
	
	bIsBerserk = true;
	if (GetEnemyAttributeSet())
	{
		GetEnemyAttributeSet()->SetBerserkGage(0.f);
	}
	UFDFunctionLibrary::RemoveGameplayFromActorIfFound(this, UFDGameplayStatic::GetNormalModeTag());
	UFDFunctionLibrary::AddGameplayTagToActorIfNone(this, UFDGameplayStatic::GetBerserkStartTag());

	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, FString::Printf(TEXT("Start Berserk: %s"), bIsBerserk ? TEXT("true") : TEXT("false")));

	// 무적 판정 넣기
	if (ASC && BerserkGE)
	{
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(BerserkGE, 1.f, ContextHandle);
		if (SpecHandle.IsValid())
		{
			ActiveBerserkGE = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(BerserkTimerHandle, this, &AFDBossCharacter::EndBerserk, 50.f, false);
	}
}

void AFDBossCharacter::EndBerserk()
{
	if (!HasAuthority()) return;

	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald,FString::Printf(TEXT("Ended Berserk: %s"), bIsBerserk ? TEXT("true") : TEXT("false")));

	bIsBerserk = false;
	UFDFunctionLibrary::RemoveGameplayFromActorIfFound(this, UFDGameplayStatic::GetBerserkModeTag());
	UFDFunctionLibrary::AddGameplayTagToActorIfNone(this, UFDGameplayStatic::GetNormalStartTag());

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(BerserkTimerHandle);
	}
	
	// 다시 게이지 증가 시작
	IncreaseBerserkGage();
}

void AFDBossCharacter::DestroyPart(const FName& BoneName)
{
	for (USkeletalMeshComponent* Part : Parts)
	{
		if (Part)
		{
			if (Part->GetAttachSocketName() == BoneName)
			{
				Part->DestroyComponent();
				
				if (FBodyInstance* BI = GetMesh()->GetBodyInstance(BoneName))
				{
					if (BI->GetBodySetup())
					{
						for (int32 i = 0; i < BI->GetBodySetup()->AggGeom.GetElementCount(); ++i)
						{
							BI->SetShapeCollisionEnabled(i, ECollisionEnabled::NoCollision);
						}
					}
				}
			}
		}
	}

}

void AFDBossCharacter::NetMulticast_TwinklePart_Implementation(const FName& BoneName)
{
	FTimerHandle TimerHandle;
	
	for (USkeletalMeshComponent* Part : Parts)
	{
		if (Part)
		{
			if (Part->GetAttachSocketName() == BoneName && BoneName != "Bn_CorePlasma")
			{
				Part->SetVectorParameterValueOnMaterials("HitEmissiveColor", FVector(FLinearColor(0.f, 0.f, 5.f, 1)));
				Part->SetScalarParameterValueOnMaterials("HitFxSwitch", 1.f);

				GetWorld()->GetTimerManager().SetTimer(TimerHandle, [Part]()
				{
					if (Part)
						Part->SetScalarParameterValueOnMaterials("HitFxSwitch", 0.f);
				}, 0.1f, false);

				break;
			}
			if (BoneName == "Bn_CorePlasma")
			{
				Plasma->SetVectorParameterValueOnMaterials("HitEmissiveColor", FVector(FLinearColor(5.f, 5.f, 5.f, 1)));
				Plasma->SetScalarParameterValueOnMaterials("HitFxSwitch", 1.f);

				GetWorld()->GetTimerManager().SetTimer(TimerHandle, [Part]()
				{
					if (Part)
						Part->SetScalarParameterValueOnMaterials("HitFxSwitch", 0.f);
				}, 0.1f, false);
			}
		}
	}
}

void AFDBossCharacter::NetMulticast_TwinklePart_Warning_Implementation(const FName& BoneName)
{
	FTimerHandle TimerHandle;
	
	for (USkeletalMeshComponent* Part : Parts)
	{
		if (Part)
		{
			if (Part->GetAttachSocketName() == BoneName && BoneName != "Bn_CorePlasma")
			{
				Part->SetVectorParameterValueOnMaterials("HitEmissiveColor", FVector(FLinearColor(5.f, 5.f, 0.f, 1)));
				Part->SetScalarParameterValueOnMaterials("HitFxSwitch", 1.f);
				
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, [Part]()
				{
					if (Part)
						Part->SetScalarParameterValueOnMaterials("HitFxSwitch", 0.f);
				}, 0.1f, false);

				break;
			}
			if (BoneName == "Bn_CorePlasma")
			{
				Plasma->SetVectorParameterValueOnMaterials("HitEmissiveColor", FVector(FLinearColor(5.f, 5.f, 0.f, 1)));
				Plasma->SetScalarParameterValueOnMaterials("HitFxSwitch", 1.f);

				GetWorld()->GetTimerManager().SetTimer(TimerHandle, [Part]()
				{
					if (Part)
						Part->SetScalarParameterValueOnMaterials("HitFxSwitch", 0.f);
				}, 0.1f, false);
			}
		}
	}
}

void AFDBossCharacter::NetMulticast_HideBossHUD_Implementation()
{
	if (AFDPlayerController* PC = Cast<AFDPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		PC->HideBossStatusHUD();
	}
}

void AFDBossCharacter::NetMulticast_OnBerserk_NS_Implementation()
{
	if (BerserkNS)
	{
		BerserkNS->Activate();
	}
}

void AFDBossCharacter::NetMulticast_OffBerserk_NS_Implementation()
{
	if (BerserkNS)
	{
		BerserkNS->Deactivate();
	}
}

void AFDBossCharacter::DestroyEndBerserk()
{
	if (!HasAuthority()) return;

	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald,FString::Printf(TEXT("Ended Berserk: %s"), bIsBerserk ? TEXT("true") : TEXT("false")));

	bIsBerserk = false;
	UFDFunctionLibrary::RemoveGameplayFromActorIfFound(this, UFDGameplayStatic::GetBerserkModeTag());
	UFDFunctionLibrary::AddGameplayTagToActorIfNone(this, UFDGameplayStatic::GetNormalModeTag());
	
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(BerserkTimerHandle);
	}
	
	// 다시 게이지 증가 시작
	IncreaseBerserkGage();
}

void AFDBossCharacter::OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* OwningPawn = GetInstigator<APawn>();
	
	if (!OwningPawn)
	{
		return;
	}
	
	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (OwningPawn != HitPawn)
		{
			OnCollisionHitTarget.ExecuteIfBound(OtherActor);
			if (UFDFunctionLibrary::IsTargetPawnHostile(OwningPawn, HitPawn))
			{
				// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *OtherActor->GetActorNameOrLabel());
				UGameplayStatics::ApplyDamage(HitPawn, 80.f, GetOwner()->GetInstigatorController(), GetOwner(), nullptr);
			}
		}
	}
		
}

void AFDBossCharacter::OnCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APawn* OwningPawn = GetInstigator<APawn>();

	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (OwningPawn != HitPawn)
		{
			OnCollisionPulledFromTarget.ExecuteIfBound(OtherActor);
		}
	}
}
