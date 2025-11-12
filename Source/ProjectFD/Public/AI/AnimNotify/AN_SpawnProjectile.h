// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_SpawnProjectile.generated.h"

class AFDProjectileBase;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UAN_SpawnProjectile : public UAnimNotify
{
	GENERATED_BODY()

public:
	// 어떤 발사체 스폰할지 지정 (AFDProjectile 기반으로 BP 만드시면 됩니다.)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile")
	TSubclassOf<AFDProjectileBase> ProjectileClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile")
	FName MuzzleSocketName = "";

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
};
