// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/Player/DBNOAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "Equipment/EquipmentActor.h"
#include "GAS/Player/PlayerAttributeSet.h"
#include "Kismet/GameplayStatics.h"


UDBNOAbility::UDBNOAbility()
{
	
}

void UDBNOAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = GetFDAbilitySystemComponentFromActorInfo();
	UPlayerAttributeSet* PlayerAttrSet = const_cast<UPlayerAttributeSet*>(ASC->GetSet<UPlayerAttributeSet>());

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	const FGameplayEffectContextHandle GEContextHandle = ASC->MakeEffectContext();
	const FGameplayEffectSpecHandle CostSpecHandle = ASC->MakeOutgoingSpec(CostGEClass, 1.f, GEContextHandle);
	
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		// 체력을 최대치로 채움
		ASC->SetNumericAttributeBase(PlayerAttrSet->GetHealthAttribute(), PlayerAttrSet->GetMaxHealth());

		// 초당 최대체력의 퍼센트 감소
		const float HealthReducePer = -(DBNOHealthDrainPerSecond / 1000) * PlayerAttrSet->GetMaxHealth();
	
		const FGameplayTag CostTag = FGameplayTag::RequestGameplayTag(TEXT("Data.DBNO.Cost"));
		CostSpecHandle.Data->SetSetByCallerMagnitude(CostTag, HealthReducePer);

		DBNOCostGEHandle = ASC->ApplyGameplayEffectSpecToSelf(*CostSpecHandle.Data.Get());

		UGameplayStatics::PlaySoundAtLocation(GetPlayerCharacterFromActorInfo(), DBNOStartSound, GetPlayerCharacterFromActorInfo()->GetActorLocation());

		UAbilityTask_PlayMontageAndWait* PlaySkillMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, DBNOStartMontage);
		PlaySkillMontageTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag(FName("Event.DBNO.Start")), nullptr, false);
		WaitEventTask->EventReceived.AddDynamic(this, &UDBNOAbility::NextMontage);
		WaitEventTask->ReadyForActivation();
	}
	
	OnStartDBNO();

	GetWorld()->GetTimerManager().SetTimer(DBNOLoopSoundTimer, this, &UDBNOAbility::PlayDBNOSoundTick, 2.0f, true);
	
	// 체력 0 콜백 바인딩 - 중복 방지
	if (PlayerAttrSet)
	{
		PlayerAttrSet->OnHealthDepleted.RemoveDynamic(this, &UDBNOAbility::OnDeath);
		PlayerAttrSet->OnHealthDepleted.AddUniqueDynamic(this, &UDBNOAbility::OnDeath);
	}
}

void UDBNOAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (AFDPlayerCharacter* PC = GetPlayerCharacterFromActorInfo())
	{
		if (UAbilitySystemComponent* ASC = GetFDAbilitySystemComponentFromActorInfo())
		{
			// 서버에서 활성화한 주기적 비용 GE 제거
			if (DBNOCostGEHandle.IsValid() && ASC->IsOwnerActorAuthoritative())
			{
				ASC->RemoveActiveGameplayEffect(DBNOCostGEHandle);
				DBNOCostGEHandle.Invalidate();
				GetWorld()->GetTimerManager().ClearTimer(DBNOLoopSoundTimer);
	
				PC->GetCharacterMovement()->MaxWalkSpeed = 800.f;

				AEquipmentActor* WeaponActor = PC->GetWeaponActor();
				if (WeaponActor)
				{
					WeaponActor->SetWeaponVisibility(true);
				}
	
				UGameplayStatics::PlaySoundAtLocation(PC, DBNOEndSound, PC->GetActorLocation());
			}
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UDBNOAbility::OnStartDBNO()
{
	AFDPlayerCharacter* PC = Cast<AFDPlayerCharacter>(GetAvatarActorFromActorInfo());
	PC->GetCharacterMovement()->MaxWalkSpeed = 50.f;
	UGameplayStatics::PlaySoundAtLocation(PC, DBNOLoopSound, PC->GetActorLocation());
	AEquipmentActor* WeaponActor = PC->GetWeaponActor();
	if (WeaponActor)
	{
		WeaponActor->SetWeaponVisibility(false);
	}
}

void UDBNOAbility::OnDeath()
{
	UAbilitySystemComponent* ASC = GetFDAbilitySystemComponentFromActorInfo();
	if (UPlayerAttributeSet* PlayerAttrSet = const_cast<UPlayerAttributeSet*>(ASC->GetSet<UPlayerAttributeSet>()))
	{
		PlayerAttrSet->OnHealthDepleted.RemoveDynamic(this, &UDBNOAbility::OnDeath);
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}


void UDBNOAbility::NextMontage(FGameplayEventData Payload)
{
	// 몽타주 재생 후 루프 몽타주 재생
	UAbilityTask_PlayMontageAndWait* PlaySkillMontageTask2 = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, DBNOLoopMontage);
	PlaySkillMontageTask2->ReadyForActivation();
}

void UDBNOAbility::PlayDBNOSoundTick()
{
	UGameplayStatics::PlaySoundAtLocation(GetPlayerCharacterFromActorInfo(), DBNOLoopSound, GetPlayerCharacterFromActorInfo()->GetActorLocation());
}
