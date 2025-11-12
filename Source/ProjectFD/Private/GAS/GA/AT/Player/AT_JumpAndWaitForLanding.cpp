// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/AT/Player/AT_JumpAndWaitForLanding.h"

#include "GameFramework/Character.h"


UAT_JumpAndWaitForLanding::UAT_JumpAndWaitForLanding()
{
}

UAT_JumpAndWaitForLanding* UAT_JumpAndWaitForLanding::CreateTask(UGameplayAbility* OwningAbility)
{
	UAT_JumpAndWaitForLanding* NewTask = NewAbilityTask<UAT_JumpAndWaitForLanding>(OwningAbility);
	return NewTask;
}

void UAT_JumpAndWaitForLanding::Activate()
{
	Super::Activate();

	ACharacter* Character = CastChecked<ACharacter>(GetAvatarActor());
	Character->LandedDelegate.AddDynamic(this, &UAT_JumpAndWaitForLanding::OnLandedCallback);
	Character->Jump();

	SetWaitingOnAvatar();
}

void UAT_JumpAndWaitForLanding::OnDestroy(bool AbilityEnded)
{
	Super::OnDestroy(AbilityEnded);

	ACharacter* Character = CastChecked<ACharacter>(GetAvatarActor());
	Character->LandedDelegate.RemoveDynamic(this, &UAT_JumpAndWaitForLanding::OnLandedCallback);
	Character->Landed(FHitResult());
}

void UAT_JumpAndWaitForLanding::OnLandedCallback(const FHitResult& Hit)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnComplete.Broadcast();
	}
}
