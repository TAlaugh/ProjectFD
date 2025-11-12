// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/Player/BunnySkillTwoAbility.h"

#include "AbilitySystemComponent.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "GAS/FDGameplayStatic.h"
#include "GAS/Player/PlayerAttributeSet.h"


UBunnySkillTwoAbility::UBunnySkillTwoAbility()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    CooldownTag = FGameplayTag::RequestGameplayTag(FName("Data.Skill.Cooldown.2"));
    bIsCancelable = true;
}

void UBunnySkillTwoAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    UAbilitySystemComponent* ASC = GetFDAbilitySystemComponentFromActorInfo();
    AFDPlayerCharacter* PC = Cast<AFDPlayerCharacter>(GetAvatarActorFromActorInfo());
    if (!ASC || !PC)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }
    
    // GE 이펙트 적용
    const FGameplayEffectContextHandle GEContextHandle = ASC->MakeEffectContext();
    const FGameplayEffectSpecHandle GrantTagSpecHandle = ASC->MakeOutgoingSpec(GrantTagGEClass, 1.f, GEContextHandle);
    const FGameplayEffectSpecHandle FirstCostSpecHandle = ASC->MakeOutgoingSpec(FirstCostGEClass, 1.f, GEContextHandle);
    const FGameplayEffectSpecHandle CostSpecHandle = ASC->MakeOutgoingSpec(CostGEClass, 1.f, GEContextHandle);
    const FGameplayEffectSpecHandle CooldownSpecHandle = ASC->MakeOutgoingSpec(CooldownGEClass, 1.f, GEContextHandle);
    UPlayerAttributeSet* PlayerAttrSet = const_cast<UPlayerAttributeSet*>(ASC->GetSet<UPlayerAttributeSet>());

    const float BunnySkillTwoCooldown = SkillCooldown * PlayerAttrSet->GetSkillCooldown();
    CooldownSpecHandle.Data->SetDuration(BunnySkillTwoCooldown, true);
    CooldownSpecHandle.Data->DynamicGrantedTags.AddTag(CooldownTag);

    
    // 마나가 0 이하인지 초기에 확인
    if (PlayerAttrSet->GetMana() < FirstCostAmount * PlayerAttrSet->GetSkillCost())
    {
        OnEndAbility();
        CancelAbility(Handle, ActorInfo, ActivationInfo, true);
        return;
    }

    // 이미 활성화된 상태면 취소
    if (ASC->HasMatchingGameplayTag(UFDGameplayStatic::GetBunnySkillTwoTag()))
    {
        OnEndAbility();
        return;
    }

    
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    const FGameplayTag FistCostTag = FGameplayTag::RequestGameplayTag(TEXT("Data.Skill.Cost"));
    const float BunnySkillTwoFirstCost = FirstCostAmount * PlayerAttrSet->GetSkillCost();
    FirstCostSpecHandle.Data->SetSetByCallerMagnitude(FistCostTag, -BunnySkillTwoFirstCost);
    
    // SetByCaller 주입: 플레이어 SkillCost를 읽어 비용을 음수로 전달(마나 차감)
    const FGameplayTag CostTag = FGameplayTag::RequestGameplayTag(TEXT("Data.Skill.Cost"));
    const float BunnySkillTwoCost = CostAmount * PlayerAttrSet->GetSkillCost();
    CostSpecHandle.Data->SetSetByCallerMagnitude(CostTag, -BunnySkillTwoCost);
    
    
    // 버프 적용
    PC->GetCharacterMovement()->MaxWalkSpeed = 1200.0f;
    ASC->ApplyGameplayEffectSpecToSelf(*GrantTagSpecHandle.Data.Get());
    ASC->ApplyGameplayEffectSpecToSelf(*FirstCostSpecHandle.Data.Get());
    ASC->ApplyGameplayEffectSpecToSelf(*CostSpecHandle.Data.Get());
    ASC->ApplyGameplayEffectSpecToSelf(*CooldownSpecHandle.Data.Get());
    
    PC->Multicast_SpawnSkillNiagaraEffectAttachedToComponent(SkillTwoNiagaraSystem, FName("Bip001-Pelvis"));
    PC->Multicast_PlaySkillSoundAtLocation(SkillTwoSoundStart, PC->GetActorLocation());
    // 0.1초 뒤에 쿨다운 알림 실행
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(CooldownTimerHandle, [this]()
            {
                AFDPlayerCharacter* PC = Cast<AFDPlayerCharacter>(GetAvatarActorFromActorInfo());
                if (HasAuthority(&CurrentActivationInfo))
                {
                    PC->NotifySkillCoolDown(CooldownTag);
                }
            },
            0.1f,
            false);
    }
    
    // 마나 0 콜백 바인딩
    PlayerAttrSet->OnManaDepleted.AddDynamic(this, &UBunnySkillTwoAbility::OnEndAbility);
    
    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UBunnySkillTwoAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility, bool bWasCancelled)
{
    //GetPlayerCharacterFromActorInfo()->Multicast_DestroySkillNiagara();
    //GetPlayerCharacterFromActorInfo()->Multicast_PlaySkillSoundAtLocation(SkillTwoSoundEnd, GetPlayerCharacterFromActorInfo()->GetActorLocation());
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBunnySkillTwoAbility::OnEndAbility()
{
    UAbilitySystemComponent* ASC = GetFDAbilitySystemComponentFromActorInfo();
    AFDPlayerCharacter* PC = Cast<AFDPlayerCharacter>(GetAvatarActorFromActorInfo());
    if (!PC) return;

    PC->Multicast_DestroySkillNiagara();
    PC->Multicast_PlaySkillSoundAtLocation(SkillTwoSoundEnd, PC->GetActorLocation());
    // 토글 오프 정리(태그/코스트 GE 제거 + 속도 감소)
    if (ASC)
    {
        FGameplayTagContainer RemoveTags;
        RemoveTags.AddTag(UFDGameplayStatic::GetBunnySkillTwoTag());
        ASC->RemoveActiveEffectsWithGrantedTags(RemoveTags);
        ASC->RemoveActiveGameplayEffectBySourceEffect(CostGEClass, ASC, -1);
    }
    
    if (PC)
    {
        PC->GetCharacterMovement()->MaxWalkSpeed = 800.0f;
    }
    
    // 델리게이트 해제
    if (ASC)
    {
        if (UPlayerAttributeSet* PlayerAttrSet = const_cast<UPlayerAttributeSet*>(ASC->GetSet<UPlayerAttributeSet>()))
        {
            PlayerAttrSet->OnManaDepleted.RemoveDynamic(this, &UBunnySkillTwoAbility::OnEndAbility);
            PC->Multicast_DestroySkillNiagara();
            EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        }
    }
    
}
