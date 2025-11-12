#pragma once

#include "CoreMinimal.h"
#include "FDGameplayAbilityTypes.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EFDAbilityInputID : uint8
{
	None				UMETA(DisplayName = "None"),
	Roll				UMETA(DisplayName = "Roll"),
	Jump				UMETA(DisplayName = "Jump"),
	AttackCheck			UMETA(DisplayName = "AttackCheck"),
	ReloadWeapon		UMETA(DisplayName = "ReloadWeapon"),
	MeleeAttack			UMETA(DisplayName = "MeleeAttack"),
	RangeAttack			UMETA(DisplayName = "RangeAttack"),
	FirstSkill			UMETA(DisplayName = "FirstSkill"),
	SecondSkill			UMETA(DisplayName = "SecondSkill"),
	ThirdSkill			UMETA(DisplayName = "ThirdSkill"),
	FourthSkill			UMETA(DisplayName = "FourthSkill"),
	WireAction			UMETA(DisplayName = "WireAction"),
	HangOnAttack		UMETA(DisplayName = "HangOnAttack"),
	HangOnGrab			UMETA(DisplayName = "HangOnGrab"),
	DBNO 				UMETA(DisplayName = "DBNO"),
	Rescue				UMETA(DisplayName = "Rescue"),
	EquipSlot1			UMETA(DisplayName = "EquipSlot1"),
	EquipSlot2			UMETA(DisplayName = "EquipSlot2"),
	EquipSlot3			UMETA(DisplayName = "EquipSlot3"),
	PassiveSkill		UMETA(DisplayName = "PassiveSkill"),
	Aim					UMETA(DisplayName = "Aim")
};