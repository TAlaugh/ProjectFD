// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Item/DropItem/DropItemBase.h"
#include "DropItemDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FDropItemData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<ADropItemBase> DropItemClass;

	UPROPERTY(EditAnywhere)
	float DropRate = 1.0f;
};

/**
 * 
 */
UCLASS()
class PROJECTFD_API UDropItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FDropItemData> MonsterDrops;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FDropItemData> BossDrops;
};
