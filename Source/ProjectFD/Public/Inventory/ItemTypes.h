
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ItemTypes.generated.h"

class UModuleDefinition;
class UEquipmentDefinition;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FItemAcquisitionInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MinLevel = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxLevel = 0;
};

USTRUCT(BlueprintType)
struct FEquipmentProps
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UEquipmentDefinition> EquipmentClass = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UAnimInstance> CharacterAnimLayer = nullptr;
	
};

USTRUCT(BlueprintType)
struct FConsumableProps
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> ItemEffectClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ItemEffectLevel = 1.f;
	
};

USTRUCT(BlueprintType)
struct FModuleProps
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UModuleDefinition> ModuleClass = nullptr;
};

USTRUCT(BlueprintType)
struct FMasterItemDefinition : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "Item"))
	FGameplayTag ItemTag = FGameplayTag::EmptyTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText ItemName = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (MultiLine = true))
	FText Description = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (MultiLine = true))
	FText AdditionalInfo = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Icon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> SmallIcon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FConsumableProps ConsumableProps = FConsumableProps();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FEquipmentProps EquipmentProps = FEquipmentProps();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FModuleProps ModuleProps = FModuleProps();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FItemAcquisitionInfo ItemAcquisitionInfo = FItemAcquisitionInfo();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ItemMaxStackCount = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "Rarity"))
	FGameplayTag RarityTag = FGameplayTag::EmptyTag;

	bool IsValid() const
	{
		return ItemTag.IsValid() && !ItemName.IsEmpty();
	}
	
};

