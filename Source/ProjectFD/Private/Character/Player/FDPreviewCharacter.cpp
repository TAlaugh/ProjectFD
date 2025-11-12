// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/FDPreviewCharacter.h"

#include "Character/Player/FDPlayerCharacter.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Equipment/EquipmentActor.h"


AFDPreviewCharacter::AFDPreviewCharacter()
{
	PreviewWeapon = nullptr;
	bReplicates = false;

	SceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent"));
	SceneCaptureComponent->SetupAttachment(GetRootComponent());
	SceneCaptureComponent->SetRelativeLocation(FVector(200.f, 0.f, 80.f));
	SceneCaptureComponent->SetRelativeRotation(FRotator(-10.f, 180.f, 0.f));
	SceneCaptureComponent->CaptureSource = ESceneCaptureSource::SCS_SceneColorSceneDepth;

	RenderTarget2D = NewObject<UTextureRenderTarget2D>();
	RenderTarget2D->InitAutoFormat(2880, 1080);
	
	SceneCaptureComponent->TextureTarget = RenderTarget2D;
}

void AFDPreviewCharacter::ChangeAnimLayer(TSubclassOf<UAnimInstance> AnimInstance)
{
	GetMesh()->GetAnimInstance()->LinkAnimClassLayers(AnimInstance);
}

void AFDPreviewCharacter::EquipPreviewWeapon(AEquipmentActor* WeaponActor)
{
	if (PreviewWeapon)
	{
		PreviewWeapon->Destroy();
		PreviewWeapon = nullptr;
	}
	if (WeaponActor)
	{
		PreviewWeapon = GetWorld()->SpawnActor<AEquipmentActor>(WeaponActor->GetClass());
		PreviewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("RHandWeaponSocket"));
	}
}

UTextureRenderTarget2D* AFDPreviewCharacter::GetRenderTarget2D()
{
	if (!RenderTarget2D)
	{
		RenderTarget2D = NewObject<UTextureRenderTarget2D>();
		RenderTarget2D->InitAutoFormat(2880, 1080);
	
		SceneCaptureComponent->TextureTarget = RenderTarget2D;
	}

	return RenderTarget2D;
}
