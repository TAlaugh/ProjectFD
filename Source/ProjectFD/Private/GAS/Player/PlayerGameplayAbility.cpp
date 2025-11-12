// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Player/PlayerGameplayAbility.h"

#include "Character/Player/FDPlayerCharacter.h"
#include "Controller/FDPlayerController.h"
#include "Kismet/KismetSystemLibrary.h"


AFDPlayerCharacter* UPlayerGameplayAbility::GetPlayerCharacterFromActorInfo()
{
	if (!CachedPlayerCharacter.IsValid())
	{
		CachedPlayerCharacter = Cast<AFDPlayerCharacter>(CurrentActorInfo->AvatarActor);
	}

	return CachedPlayerCharacter.IsValid() ? CachedPlayerCharacter.Get() : nullptr;
}

AFDPlayerController* UPlayerGameplayAbility::GetPlayerControllerFromActorInfo()
{
	if (!CachedPlayerController.IsValid())
	{
		CachedPlayerController = Cast<AFDPlayerController>(CurrentActorInfo->PlayerController);
	}

	return CachedPlayerController.IsValid() ? CachedPlayerController.Get() : nullptr;
}

UPlayerCombatComponent* UPlayerGameplayAbility::GetPlayerCombatComponentFromActorInfo()
{
	return GetPlayerCharacterFromActorInfo()->GetCombatComponent();
}

TArray<FHitResult> UPlayerGameplayAbility::GetHitResultFromSphereSweepLocationTargetData(
	const FGameplayAbilityTargetDataHandle& TargetDataHandle, float SphereSweetRadius, float TargetAmount, bool bDrawDebug,
	ETeamAttitude::Type TargetTeam, bool bIgnoreSelf) const
{
	TArray<FHitResult> OutResult;
	TSet<AActor*> HitActors;
	
	
	// 설정한 값만큼만 감지(0 이하면 무제한)
	const int32 MaxCount = FMath::Max(0, FMath::FloorToInt(TargetAmount));
	int32 CollectedCount = 0;
	bool bReachedLimit = false;
	
	IGenericTeamAgentInterface* OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo());
	

	for (const TSharedPtr<FGameplayAbilityTargetData> TargetData : TargetDataHandle.Data)
	{
		if (bReachedLimit)
		{
			break;
		}
		if (!TargetData.IsValid())
		{
			continue;
		}
		
		FVector StartLocation = TargetData->GetOrigin().GetTranslation();
		FVector EndLocation = TargetData->GetEndPoint();
		
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

		TArray<AActor*> ActorToIgnore;
		if (bIgnoreSelf)
		{
			ActorToIgnore.Add(GetAvatarActorFromActorInfo());
		}

		EDrawDebugTrace::Type DrawDebugTrace = bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
		TArray<FHitResult> Results;
		UKismetSystemLibrary::SphereTraceMultiForObjects(this,StartLocation, EndLocation,
			 SphereSweetRadius, ObjectTypes,false,ActorToIgnore,DrawDebugTrace,Results, false,FLinearColor::Green, FLinearColor::Red, DrawDebugDuration);

		for (const FHitResult& Result : Results)
		{
			if (bReachedLimit)
			{
				break;
			}

			AActor* HitActor = Result.GetActor();
			if (!HitActor || HitActors.Contains(HitActor))
			{
				continue;
			}

			if (OwnerTeamInterface)
			{
				const ETeamAttitude::Type OtherActorTeamAttitude = OwnerTeamInterface->GetTeamAttitudeTowards(*HitActor);
				if (OtherActorTeamAttitude != TargetTeam)
				{
					continue;
				}
			}

			HitActors.Add(HitActor);
			OutResult.Add(Result);

			++CollectedCount;
			if (MaxCount > 0 && CollectedCount >= MaxCount)
			{
				bReachedLimit = true;
				break;
			}
		}
	}

	return OutResult;
}

TArray<FHitResult> UPlayerGameplayAbility::GetHitResultFromCapsuleSweepLocationTargetData(
	const FGameplayAbilityTargetDataHandle& TargetDataHandle, float CapsuleHalfHeight, float CapsuleRadius,
	float TargetAmount, bool bDrawDebug, ETeamAttitude::Type TargetTeam, bool bIgnoreSelf) const
{
   TArray<FHitResult> OutResult;
    TSet<AActor*> HitActors;

    const int32 MaxCount = FMath::Max(0, FMath::FloorToInt(TargetAmount));
    int32 CollectedCount = 0;
    bool bReachedLimit = false;

    IGenericTeamAgentInterface* OwnerTeamInterface =
        Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo());

    for (const TSharedPtr<FGameplayAbilityTargetData>& TargetData : TargetDataHandle.Data)
    {
        if (bReachedLimit)
        {
            break;
        }
        if (!TargetData.IsValid())
        {
            continue;
        }

        const FVector StartLocation = TargetData->GetOrigin().GetTranslation();
        const FVector EndLocation = TargetData->GetEndPoint();

        TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
        ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

        TArray<AActor*> ActorToIgnore;
        if (bIgnoreSelf)
        {
            ActorToIgnore.Add(GetAvatarActorFromActorInfo());
        }

        const EDrawDebugTrace::Type DrawDebugTrace =
            bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

        TArray<FHitResult> Results;
        UKismetSystemLibrary::CapsuleTraceMultiForObjects(
            this,
            StartLocation,
            EndLocation,
            CapsuleRadius,
            CapsuleHalfHeight,
            ObjectTypes,
            false,
            ActorToIgnore,
            DrawDebugTrace,
            Results,
            false // 내부에서 이미 자기 자신을 무시했으므로 false 유지
        );

        for (const FHitResult& Result : Results)
        {
            if (bReachedLimit)
            {
                break;
            }

            AActor* HitActor = Result.GetActor();
            if (!HitActor || HitActors.Contains(HitActor))
            {
                continue;
            }

            if (OwnerTeamInterface)
            {
                const ETeamAttitude::Type OtherAttitude =
                    OwnerTeamInterface->GetTeamAttitudeTowards(*HitActor);
                if (OtherAttitude != TargetTeam)
                {
                    continue;
                }
            }

            HitActors.Add(HitActor);
            OutResult.Add(Result);

            ++CollectedCount;
            if (MaxCount > 0 && CollectedCount >= MaxCount)
            {
                bReachedLimit = true;
                break;
            }
        }
    }

    return OutResult;
}
