// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FDGameplayStatic.generated.h"

class UAbilitySystemComponent;
class UFDAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UFDGameplayStatic : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	// Player Ability Tags
	static FGameplayTag GetMeleeAttackAbilityTag();
	static FGameplayTag GetRangeAttackAbilityTag();
	static FGameplayTag GetEquipWeaponAbilityTag();
	static FGameplayTag GetUnequipWeaponAbilityTag();
	static FGameplayTag GetOwnerCombatTag();
	static FGameplayTag GetOwnerNormalTag();
	static FGameplayTag GetDeadStatusTag();
	static FGameplayTag GetOwnerAimingTag();
	static FGameplayTag GetHitWeakPointTag();
	static FGameplayTag GetBunnySkillOneTag();
	static FGameplayTag GetBunnySkillTwoTag();
	static FGameplayTag GetOwnerHangOnTag();
	static FGameplayTag GetDBNOStatusTag();

	// 플레이어의 약점 속성(배율) 가져오기
	static float GetWeakDamage(AActor* Actor);

	template<typename T>
	static T* GetDataTableRowByTag(UDataTable* DataTable, FGameplayTag Tag);

	static bool GrantGameplayTagByEffect(UAbilitySystemComponent* ASC, FGameplayTag Tag);
	static bool ActorHasGameplayTag(AActor* Actor, FGameplayTag Tag);

	UFUNCTION(BlueprintCallable)
	static bool TryGetCooldownTimeRemaining(UAbilitySystemComponent* ASC, const FGameplayTag& CooldownTag, float& OutTimeRemaining, float& OutTotalTime);
	
	// Monster Tag
	static FGameplayTag GetBerserkModeTag();
	static FGameplayTag GetBerserkStartTag();
	static FGameplayTag GetNormalModeTag();
	static FGameplayTag GetNormalStartTag();
	static FGameplayTag GetBossSpawn();
	static FGameplayTag StopOrientToRotation();
	static FGameplayTag PlayDestroyAM();
	static FGameplayTag CoreShellDestroy();
	static FGameplayTag BerserkGageBlock();
	static FGameplayTag GetDestroyClav_L();
	static FGameplayTag GetDestroyClav_R();
	static FGameplayTag MonsterGroggy();
	static FGameplayTag GetMonsterDeadTag();
	// Attach Tag
	static FGameplayTag PlayerCantAttach();
	static FGameplayTag PlayerAttachParts();
	static FGameplayTag BodySwing();

	UFUNCTION()
	FName AttachBossPart(FHitResult& HitResult);
};

template <typename T>
T* UFDGameplayStatic::GetDataTableRowByTag(UDataTable* DataTable, FGameplayTag Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(), FString(""));
}
