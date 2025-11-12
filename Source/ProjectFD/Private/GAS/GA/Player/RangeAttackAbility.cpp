// Source/ProjectFD/Private/GAS/GA/Player/GA_RangeWeaponAttack.cpp

// c++
#include "GAS/GA/Player/RangeAttackAbility.h"

#include "GAS/FDGameplayStatic.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GAS/FDAbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "GenericTeamAgentInterface.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "Equipment/EquipmentActor.h"
#include "GameFramework/DamageType.h"
#include "GAS/Player/PlayerAttributeSet.h"

URangeAttackAbility::URangeAttackAbility()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    bRetriggerInstancedAbility = true;
}

bool URangeAttackAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
    const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
    if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
    {
        UE_LOG(LogTemp, Warning, TEXT("CanActivateAbility: Super::CanActivateAbility() failed"));
        return false;
    }

    const UFDAbilitySystemComponent* AbilitySystemComponent = Cast<UFDAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());
    const UPlayerAttributeSet* PlayerAttrSet = AbilitySystemComponent ? AbilitySystemComponent->GetSet<UPlayerAttributeSet>() : nullptr;
    if (!PlayerAttrSet || PlayerAttrSet->GetAmmo() <= 0)
    {
        return false;
    }

    return true;
}


void URangeAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    ASC = Cast<UFDAbilitySystemComponent>(ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr);

    ACharacter* Character = ActorInfo ? Cast<ACharacter>(ActorInfo->AvatarActor.Get()) : nullptr;
    if (!Character)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }
    
    AEquipmentActor* Equipment = nullptr;
    {
        TArray<AActor*> Attached;
        Character->GetAttachedActors(Attached);
        for (AActor* A : Attached)
        {
            if ((Equipment = Cast<AEquipmentActor>(A)) != nullptr)
            {
                break;
            }
        }
    }
    
    OnStartMontageCompleted();
}


void URangeAttackAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility, bool bWasCancelled)
{
    // Ability.Aim 태그가 없으면 Fire 몽타주를 수동으로 정지
    if (ASC && !ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Ability.Aim"))))
    {
        if (const FGameplayAbilityActorInfo* Info = GetCurrentActorInfo())
        {
            if (ACharacter* Character = Cast<ACharacter>(Info->AvatarActor.Get()))
            {
                if (USkeletalMeshComponent* Mesh = Character->GetMesh())
                {
                    if (UAnimInstance* Anim = Mesh->GetAnimInstance())
                    {
                        if (FireMontage && Anim->Montage_IsPlaying(FireMontage))
                        {
                            Anim->Montage_Stop(0.2f, FireMontage);
                        }
                    }
                }
            }
        }
    }
    
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
    Cast<AFDPlayerCharacter>(GetCurrentActorInfo()->AvatarActor.Get())->FireWeapon(false);
}

void URangeAttackAbility::InputReleased(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
    Super::InputReleased(Handle, ActorInfo, ActivationInfo);
    Cast<AFDPlayerCharacter>(GetCurrentActorInfo()->AvatarActor.Get())->FireWeapon(false);
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void URangeAttackAbility::InputPressed(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
    Super::InputPressed(Handle, ActorInfo, ActivationInfo);
}

void URangeAttackAbility::OnEndAbility()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

#pragma region -- TraceAndDamage--
bool URangeAttackAbility::DoLineTrace(const FGameplayAbilityActorInfo* ActorInfo, FHitResult& OutHit,
                                      FVector& OutStart, FVector& OutEnd) const
{
    OutHit = FHitResult{};
    OutStart = FVector::ZeroVector;
    OutEnd = FVector::ZeroVector;

    if (!ActorInfo) return false;

    const ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
    if (!Character) return false;

    UWorld* World = ActorInfo->AvatarActor.IsValid() ? ActorInfo->AvatarActor->GetWorld() : nullptr;
    if (!World) return false;

    // 1) 총구 위치 찾기
    USkeletalMeshComponent* WeaponMesh = nullptr;
    {
        TArray<USkeletalMeshComponent*> SkelMeshes;
        Character->GetComponents(SkelMeshes);
        for (USkeletalMeshComponent* Comp : SkelMeshes)
        {
            if (!Comp || Comp == Character->GetMesh()) continue;
            const bool bHasSocket = Comp->DoesSocketExist(MuzzleSocketName);
            const bool bHasBone   = Comp->GetBoneIndex(MuzzleSocketName) != INDEX_NONE;
            if (bHasSocket || bHasBone)
            {
                WeaponMesh = Comp;
                break;
            }
        }
    }

    auto TryGetWorldLocation = [](const USkeletalMeshComponent* Skel, const FName& Name, FVector& OutLoc) -> bool
    {
        if (!Skel) return false;

        if (Skel->DoesSocketExist(Name))
        {
            OutLoc = Skel->GetSocketLocation(Name);
            return true;
        }

        const int32 BoneIdx = Skel->GetBoneIndex(Name);
        if (BoneIdx != INDEX_NONE)
        {
            FTransform BoneTM = Skel->GetBoneTransform(BoneIdx) * Skel->GetComponentToWorld();
            OutLoc = BoneTM.GetLocation();
            return true;
        }
        return false;
    };

    FVector MuzzleStart = Character->GetActorLocation();
    if (!(WeaponMesh && TryGetWorldLocation(WeaponMesh, MuzzleSocketName, MuzzleStart)))
    {
        if (!TryGetWorldLocation(Character->GetMesh(), MuzzleSocketName, MuzzleStart))
        {
            MuzzleStart = Character->GetActorLocation();
        }
    }

    // 2) 카메라(화면 중앙)로 1차 에임 트레이스
    FVector CamLoc; 
    FRotator CamRot;
    if (const AController* PC = Character->GetController())
    {
        PC->GetPlayerViewPoint(CamLoc, CamRot);
    }
    else
    {
        Character->GetActorEyesViewPoint(CamLoc, CamRot);
    }

    FCollisionQueryParams AimParams(SCENE_QUERY_STAT(GA_RangeWeaponAttack_AimTrace), /*bTraceComplex*/ true, Character);
    {
        AimParams.AddIgnoredActor(const_cast<ACharacter*>(Character));
        TArray<AActor*> Attached;
        Character->GetAttachedActors(Attached);
        AimParams.AddIgnoredActors(Attached);
    }

    const FVector AimEnd = CamLoc + CamRot.Vector() * TraceDistance;
    FHitResult AimHit;
    const bool bAimHit = World->LineTraceSingleByChannel(AimHit, CamLoc, AimEnd, ECC_Visibility, AimParams);
    const FVector AimPoint = bAimHit ? AimHit.ImpactPoint : AimEnd;

    // 3) 총구에서 AimPoint로 2차 실제 탄 트레이스
    FCollisionQueryParams ShotParams(SCENE_QUERY_STAT(GA_RangeWeaponAttack_ShotTrace), /*bTraceComplex*/ true, Character);
    {
        ShotParams.AddIgnoredActor(const_cast<ACharacter*>(Character));
        TArray<AActor*> Attached;
        Character->GetAttachedActors(Attached);
        ShotParams.AddIgnoredActors(Attached);
    }

    FVector ShotDir = (AimPoint - MuzzleStart).GetSafeNormal();
    if (ShotDir.IsNearlyZero())
    {
        ShotDir = CamRot.Vector();
    }

    const FVector ShotEnd = MuzzleStart + ShotDir * TraceDistance;
    const bool bHit = World->LineTraceSingleByChannel(OutHit, MuzzleStart, ShotEnd, ECC_Visibility, ShotParams);

    OutStart = MuzzleStart;
    OutEnd = ShotEnd;
    return bHit;
}

void URangeAttackAbility::ApplyDamageFromHit(FHitResult& Hit, const ACharacter* SourceCharacter) const
{
    if (!Hit.GetActor() || !SourceCharacter) return;

    AActor* HitActor = Hit.GetActor();

    const uint8 SourceTeam = GetTeamIdFromActorSafe(SourceCharacter);
    const uint8 TargetTeam = GetTeamIdFromActorSafe(HitActor);
    if (SourceTeam != 255 && TargetTeam != 255 && SourceTeam == TargetTeam)
    {
        return;
    }
    const UPlayerAttributeSet* PlayerAttrSet = ASC->GetSet<UPlayerAttributeSet>();
    float WeaponCriticalDamage =  PlayerAttrSet->GetWeaponCriticalDamage();
    float WeaponCriticalChance = PlayerAttrSet->GetWeaponCriticalChance();
    float DamageAmount = PlayerAttrSet->GetWeaponDamage();

    WeaponCriticalChance = FMath::Clamp(WeaponCriticalChance, 0.f, 1.f);
    bool bCriticalHit = FMath::FRand() <= WeaponCriticalChance;
    
    if (bCriticalHit)
    {
        DamageAmount *= WeaponCriticalDamage;
    }

    const FVector ShotDir = (Hit.TraceEnd - Hit.TraceStart).IsNearlyZero()
        ? SourceCharacter->GetActorForwardVector()
        : (Hit.TraceEnd - Hit.TraceStart).GetSafeNormal();

    Hit.bBlockingHit = false;
    UGameplayStatics::ApplyPointDamage(
        HitActor,
        DamageAmount,
        ShotDir,
        Hit,
        SourceCharacter->GetController(),
        const_cast<ACharacter*>(SourceCharacter),
        UDamageType::StaticClass());
}

void URangeAttackAbility::Server_ApplyHit_Implementation(const FHitResult& Hit)
{
    const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();
    if (!ActorInfo || !ActorInfo->IsNetAuthority()) return;

    ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
    if (!Character || !Hit.GetActor()) return;

    FHitResult FinalHit = Hit;

    if (bServerReTrace)
    {
        FHitResult ServerHit;
        FVector Start, End;
        const bool bServerGotHit = DoLineTrace(ActorInfo, ServerHit, Start, End);
        if (!bServerGotHit || ServerHit.GetActor() != Hit.GetActor())
        {
            return;
        }
        FinalHit = ServerHit;
    }

    ApplyDamageFromHit(FinalHit, Character);
}


uint8 URangeAttackAbility::GetTeamIdFromActorSafe(const AActor* InActor)
{
    if (!InActor)
    {
        return uint8(255);
    }

    if (const IGenericTeamAgentInterface* TeamIF_Actor = Cast<IGenericTeamAgentInterface>(InActor))
    {
        return TeamIF_Actor->GetGenericTeamId().GetId();
    }

    const APawn* P = Cast<APawn>(InActor);
    if (!P) return uint8(255);

    const AController* C = P->GetController();
    if (C)
    {
        if (const IGenericTeamAgentInterface* TeamIF_Ctrl = Cast<IGenericTeamAgentInterface>(C))
        {
            return TeamIF_Ctrl->GetGenericTeamId().GetId();
        }

        if (const APlayerState* PS_Ctrl = C->PlayerState)
        {
            if (const IGenericTeamAgentInterface* TeamIF_PS = Cast<IGenericTeamAgentInterface>(PS_Ctrl))
            {
                return TeamIF_PS->GetGenericTeamId().GetId();
            }
        }
    }

    if (const APlayerState* PS_Pawn = P->GetPlayerState())
    {
        if (const IGenericTeamAgentInterface* TeamIF_PS2 = Cast<IGenericTeamAgentInterface>(PS_Pawn))
        {
            return TeamIF_PS2->GetGenericTeamId().GetId();
        }
    }

    return uint8(255);
}

void URangeAttackAbility::PerformLineTraceAndDamage(const FGameplayAbilityActorInfo* ActorInfo, bool bApplyDamage)
{
    if (!ActorInfo) return;
    if (!ActorInfo->IsNetAuthority()) return;

    const ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
    if (!Character) return;

    FHitResult Hit;
    FVector Start, End;
    const bool bHit = DoLineTrace(ActorInfo, Hit, Start, End);

    if (UWorld* World = Character->GetWorld())
    {
        if (ShouldDebugDraw())
        {
            const FColor LineColor = bHit ? FColor::Red : FColor::Green;
            DrawDebugLine(World, Start, End, LineColor, false, 1.0f, 0, 1.5f);
            if (bHit)
            {
                DrawDebugPoint(World, Hit.ImpactPoint, 12.f, FColor::Yellow, false, 1.0f);
            }
        }
    }

    AFDPlayerCharacter* PC = Cast<AFDPlayerCharacter>(GetAvatarActorFromActorInfo());
    FGameplayTag AmmoType = PC->GetActiveWeaponAmmoType();
    if (AmmoType.MatchesTagExact(FGameplayTag::RequestGameplayTag("Equipment.AmmoType.General")))
    {
        PC->Multicast_SpawnSkillNiagaraEffectAttachedToComponent(MGMuzzleNiagaraEffect, MuzzleSocketName);
        if (bHit)
        {
            PC->Multicast_SpawnSkillNiagaraEffectAtLocation(MGHitNiagaraEffect, Hit.ImpactPoint, PC->GetActorLocation());
        }
        PC->Multicast_PlaySkillSoundAtLocation(MGFireSound, PC->GetActorLocation());
    }
    else
    {
        PC->Multicast_SpawnSkillNiagaraEffectAttachedToComponent(LNCMuzzleNiagaraEffect, MuzzleSocketName);
        if (bHit)
        {
            PC->Multicast_SpawnSkillNiagaraEffectAtLocation(LNCHitNiagaraEffect, Hit.ImpactPoint, PC->GetActorLocation());
        }
        PC->Multicast_PlaySkillSoundAtLocation(LNCFireSound, PC->GetActorLocation());
    }

    if (!bHit) return;

    if (bApplyDamage)
    {
        ApplyDamageFromHit(Hit, Character);
    }
}

#pragma endregion

void URangeAttackAbility::OnStartMontageCompleted()
{
    if (UAbilityTask_PlayMontageAndWait* FireTask =
            UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, FireMontage, 1.f, NAME_None, false))
    {
        FireTask->OnCompleted.AddDynamic(this, &URangeAttackAbility::OnEndAbility);
        FireTask->OnBlendOut.AddDynamic(this, &URangeAttackAbility::OnEndAbility);
        FireTask->OnCancelled.AddDynamic(this, &URangeAttackAbility::OnEndAbility);
        FireTask->OnInterrupted.AddDynamic(this, &URangeAttackAbility::OnEndAbility);
        FireTask->ReadyForActivation();

        UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag(FName("Event.Weapon.Fire")), nullptr, false, true);
        WaitGameplayEventTask->EventReceived.AddDynamic(this, &URangeAttackAbility::OnLineTraceAndDamage);
        WaitGameplayEventTask->ReadyForActivation();
        Cast<AFDPlayerCharacter>(GetCurrentActorInfo()->AvatarActor.Get())->FireWeapon(true);
    }
    else
    {
        OnEndAbility();
    }
}

void URangeAttackAbility::OnLineTraceAndDamage(FGameplayEventData Payload)
{
    PerformLineTraceAndDamage(GetCurrentActorInfo(), true);
    FGameplayEffectContextHandle GEContextHandle = ASC->MakeEffectContext();
    FGameplayEffectSpecHandle GrantTagSpecHandle = ASC->MakeOutgoingSpec(AmmoGEClass, 1.f, GEContextHandle);
    ASC->ApplyGameplayEffectSpecToSelf(*GrantTagSpecHandle.Data.Get());

    const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();
    AFDPlayerCharacter* PC = CastChecked<AFDPlayerCharacter>(ActorInfo->AvatarActor.Get());
    ASC = Cast<UFDAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());
    const UPlayerAttributeSet* PlayerAttrSet = ASC->GetSet<UPlayerAttributeSet>();
    if (PlayerAttrSet->GetAmmo() <= 0 )
    {
        PC->FireWeapon(false);
        OnEndAbility();
    }
}
