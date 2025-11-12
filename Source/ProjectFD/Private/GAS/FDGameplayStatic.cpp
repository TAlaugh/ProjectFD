// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/FDGameplayStatic.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Character/Monster/Boss/FDBossCharacter.h"
#include "GAS/FDAbilitySystemComponent.h"
#include "GAS/GE/Tagging/AddingTagGameplayEffect.h"
#include "GAS/Player/PlayerAttributeSet.h"


FGameplayTag UFDGameplayStatic::GetMeleeAttackAbilityTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Ability.MeleeAttack"));
}

FGameplayTag UFDGameplayStatic::GetRangeAttackAbilityTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Ability.RangeAttack"));
}

FGameplayTag UFDGameplayStatic::GetEquipWeaponAbilityTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Ability.Equip"));
}

FGameplayTag UFDGameplayStatic::GetUnequipWeaponAbilityTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Ability.Unequip"));
}

FGameplayTag UFDGameplayStatic::GetOwnerCombatTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Player.Status.Combat"));
}

FGameplayTag UFDGameplayStatic::GetOwnerNormalTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Player.Status.Normal"));
}

FGameplayTag UFDGameplayStatic::GetDeadStatusTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Player.Status.Dead"));
}

FGameplayTag UFDGameplayStatic::GetOwnerAimingTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Player.Status.Aiming"));
}

FGameplayTag UFDGameplayStatic::GetHitWeakPointTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Enemy.Boss.WeakPoint"));
}

FGameplayTag UFDGameplayStatic::GetBunnySkillOneTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Ability.BunnySkill.One"));
}

FGameplayTag UFDGameplayStatic::GetBunnySkillTwoTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Ability.BunnySkill.Two"));
}

FGameplayTag UFDGameplayStatic::GetOwnerHangOnTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Player.Status.HangOn"));
}

FGameplayTag UFDGameplayStatic::GetDBNOStatusTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Player.Status.DBNO"));
}

float UFDGameplayStatic::GetWeakDamage(AActor* Actor)
{
	if (!Actor)
	{
		return 0.0f;
	}
	
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor);
	
	if (!ASC)
	{
		return 0.0f;
	}
	
	const UPlayerAttributeSet* PlayerAttrSet = ASC->GetSet<UPlayerAttributeSet>();
	
	if (!PlayerAttrSet)
	{
		return 0.0f;
	}
	
	return PlayerAttrSet->GetWeakDamage();
}

bool UFDGameplayStatic::GrantGameplayTagByEffect(UAbilitySystemComponent* ASC, FGameplayTag Tag)
{
	if (ASC->GetAvatarActor()->HasAuthority())
	{
		TSubclassOf<UAddingTagGameplayEffect> EffectClass = UAddingTagGameplayEffect::StaticClass();
		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(EffectClass, 1.0f, Context);

		if (SpecHandle.IsValid())
		{
			SpecHandle.Data->DynamicGrantedTags.AddTag(Tag);

			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

			return true;
		}		
	}

	return false;
}

bool UFDGameplayStatic::ActorHasGameplayTag(AActor* Actor, FGameplayTag Tag)
{
	UAbilitySystemComponent* ASC = Actor->FindComponentByClass<UAbilitySystemComponent>();
	if (ASC)
	{
		return ASC->HasMatchingGameplayTag(Tag);
	}
	return false;
}

bool UFDGameplayStatic::TryGetCooldownTimeRemaining(UAbilitySystemComponent* ASC, const FGameplayTag& CooldownTag,
												float& OutRemainingTime, float& OutTotalTime)
{
	OutRemainingTime = 0.f;
	OutTotalTime     = 0.f;

	if (!ASC || !CooldownTag.IsValid())
	{
		return false;
	}
	if (!ASC->HasMatchingGameplayTag(CooldownTag))
	{
		return false;
	}
	
	const float WorldTime = ASC->GetWorld()->GetTimeSeconds();

	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(CooldownTag);

	// 쿨다운 태그를 소유(Grant/Owning)하는 GE 검색
	FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(TagContainer);
	TArray<FActiveGameplayEffectHandle> Handles = ASC->GetActiveEffects(Query);

	float BestRemaining = -1.f;
	float BestTotal = 0.f;

	for (const FActiveGameplayEffectHandle& H : Handles)
	{
		const FActiveGameplayEffect* Active = ASC->GetActiveGameplayEffect(H);
		if (!Active) continue;

		const float Duration  = Active->GetDuration();                 // 총 시간 (∞ 일 수도 있음)
		const float Remaining = Active->GetTimeRemaining(WorldTime);    // 남은 시간 (마이너스면 무의미)

		if (Remaining >= 0.f)
		{
			if (BestRemaining < 0.f || Remaining < BestRemaining)
			{
				BestRemaining = Remaining;
				BestTotal = Duration;
			}
		}
	}

	if (BestRemaining < 0.f)
		return false;

	OutRemainingTime = BestRemaining;
	OutTotalTime = BestTotal;
	return OutRemainingTime > 0.f;
}

FGameplayTag UFDGameplayStatic::GetBerserkModeTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Enemy.Boss.State.Berserk"));
}

FGameplayTag UFDGameplayStatic::GetBerserkStartTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Enemy.Boss.Start.Berserk"));
}

FGameplayTag UFDGameplayStatic::GetNormalModeTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Enemy.Boss.State.Normal"));
}

FGameplayTag UFDGameplayStatic::GetNormalStartTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Enemy.Boss.Start.Normal"));
}

FGameplayTag UFDGameplayStatic::GetBossSpawn()
{
	return FGameplayTag::RequestGameplayTag(FName("Enemy.Boss.Spawn"));
}

FGameplayTag UFDGameplayStatic::StopOrientToRotation()
{
	return FGameplayTag::RequestGameplayTag(FName("Enemy.Shared.Status.StopOrientToTarget"));
}

FGameplayTag UFDGameplayStatic::PlayDestroyAM()
{
	return FGameplayTag::RequestGameplayTag(FName("Enemy.Boss.Destroy.AM"));
}

FGameplayTag UFDGameplayStatic::CoreShellDestroy()
{
	return FGameplayTag::RequestGameplayTag(FName("Enemy.Boss.Destroy.Part.CoreShell"));
}

FGameplayTag UFDGameplayStatic::BerserkGageBlock()
{
	return FGameplayTag::RequestGameplayTag(FName("Enemy.Boss.State.BerserkGageBlock"));
}

FGameplayTag UFDGameplayStatic::GetDestroyClav_L()
{
	return FGameplayTag::RequestGameplayTag(FName("Enemy.Boss.Destroy.Part.Clav_L"));
}

FGameplayTag UFDGameplayStatic::GetDestroyClav_R()
{
	return FGameplayTag::RequestGameplayTag(FName("Enemy.Boss.Destroy.Part.Clav_R"));
}

FGameplayTag UFDGameplayStatic::MonsterGroggy()
{
	return  FGameplayTag::RequestGameplayTag(FName("Enemy.Boss.Status.Groggy"));
}

FGameplayTag UFDGameplayStatic::GetMonsterDeadTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Enemy.Shared.Status.Dead"));
}

FGameplayTag UFDGameplayStatic::PlayerCantAttach()
{
	return FGameplayTag::RequestGameplayTag(FName("Enemy.Attach.PlayerCantAttach"));
}

FGameplayTag UFDGameplayStatic::PlayerAttachParts()
{
	return FGameplayTag::RequestGameplayTag(FName("Enemy.Attach.PlayerAttachParts"));
}

FGameplayTag UFDGameplayStatic::BodySwing()
{
	return FGameplayTag::RequestGameplayTag(FName("Enemy.Attach.BodySwing"));
}

FName UFDGameplayStatic::AttachBossPart(FHitResult& HitResult)
{
	if (AFDBossCharacter* Boss = Cast<AFDBossCharacter>(HitResult.GetActor()))
	{
		for	(FBossPart& Part : Boss->BossPartsInfo)
		{
			if (Part.bCanAttach && HitResult.BoneName == Part.BoneName)
			{
				return Part.AttachBoneName;
			}
		}
	}
	
	return FName();
}