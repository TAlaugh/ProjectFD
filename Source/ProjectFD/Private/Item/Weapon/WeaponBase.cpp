// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/WeaponBase.h"


// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	//복제 허용
	bReplicates = true;
	
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
	
	WeaponMesh->SetCollisionProfileName(FName("NoCollision"));

}

USkeletalMeshComponent* AWeaponBase::GetMesh() const
{
	return WeaponMesh;
}


