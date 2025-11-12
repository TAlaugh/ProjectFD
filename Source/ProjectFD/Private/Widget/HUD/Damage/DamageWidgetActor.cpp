// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/HUD/Damage/DamageWidgetActor.h"

#include "Character/Monster/FDMonsterCharacter.h"
#include "Components/WidgetComponent.h"
#include "Widget/HUD/DamageText.h"


// Sets default values
ADamageWidgetActor::ADamageWidgetActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>("DamageWidget");
	WidgetComponent->SetupAttachment(RootComponent);
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	WidgetComponent->SetDrawSize(FVector2D(100, 50));

	WidgetComponent->SetWidgetClass(WidgetClass);
}

void ADamageWidgetActor::BeginPlay()
{
	Super::BeginPlay();

	TWeakObjectPtr<ADamageWidgetActor> WeakSelf(this);
	
	FTimerHandle DestroyHandle;
	GetWorld()->GetTimerManager().SetTimer(DestroyHandle, [WeakSelf]()
	{
		if (WeakSelf.IsValid())
		{
			WeakSelf->Destroy();
		}
	}, 0.3f, false);
}

void ADamageWidgetActor::InitDamage(float Damage)
{
	if (!HasAuthority())
	{
		return;
	}
	
	if (!WidgetComponent) return;

	if (UDamageText* DamageWidget = Cast<UDamageText>(WidgetComponent->GetUserWidgetObject()))
	{
		DamageWidget->SetDamageText(Damage);
	}
}


