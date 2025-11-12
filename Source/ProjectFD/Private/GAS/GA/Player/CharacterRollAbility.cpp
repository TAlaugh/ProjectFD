// C++
#include "GAS/GA/Player/CharacterRollAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/Player/FDPlayerCharacter.h"

void UCharacterRollAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AFDPlayerCharacter* Character = Cast<AFDPlayerCharacter>(ActorInfo->AvatarActor.Get());
	bool IsFalling = false;
	if(Character)
	{
		IsFalling = Character->GetMovementComponent()->IsFalling();
	}
		
	//능력이 적용이 문제가 되면 (자원부족...cost) or Jumping 상태시 탈출
	if (!CommitAbility(Handle,ActorInfo,ActivationInfo) || IsFalling)
	{
		UE_LOG(LogTemp, Warning, TEXT("ActivateAbility 탈출"));
		EndAbility(Handle, ActorInfo, ActivationInfo,true, false);
		return;
	}
	
	if (RollMontage && ActorInfo->AvatarActor.IsValid())
	{
		// 어빌리티 태스크로 몽타주 실행 (자동 동기화)
		UAbilityTask_PlayMontageAndWait* MontageTask =
			UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this, NAME_None, RollMontage, 1.0f
			);
		MontageTask->ReadyForActivation();
		
		if (Character && Character->HasAuthority())
		{
			TotalRollDistance = 600.0f;
			RollElapsedTime = 0.0f;
			RollDuration = FMath::Max(0.01f, RollMontage->GetPlayLength()*0.6f); // 몽타주 길이의 60% 동안 롤 이동, 0으로 나누는 상황 방지
			RollCharacter = Character;

			RollCharacter->Multicast_PlaySkillSoundAtLocation(RollSound, RollCharacter->GetActorLocation());
			Character->GetWorldTimerManager().SetTimer(
				RollTimerHandle, this, &UCharacterRollAbility::TickRollMove, 0.01f, true
			);
		}
	}
}

void UCharacterRollAbility::TickRollMove()
{
	if (!RollCharacter)
		return;

	// 프레임 델타로 정확한 시간 누적 — 고정 0.01f 의존 제거
	UWorld* World = RollCharacter->GetWorld();
	if (!World)
		return;

	float Delta = World->GetDeltaSeconds();
	if (Delta <= 0.0f)
		return;

	float PrevElapsed = RollElapsedTime;
	RollElapsedTime += Delta;

	// 안전한 분모 처리
	float Duration = FMath::Max(0.0001f, RollDuration);
	float Alpha = FMath::Clamp(RollElapsedTime / Duration, 0.0f, 1.0f);
	float PrevAlpha = FMath::Clamp(PrevElapsed / Duration, 0.0f, 1.0f);

	// 부드러운 감속: ease-out cubic
	auto EaseOutCubic = [](float t) -> float { return 1.0f - FMath::Pow(1.0f - t, 3.0f); };

	float EasedAlpha = EaseOutCubic(Alpha);
	float EasedPrevAlpha = EaseOutCubic(PrevAlpha);

	float CurrentDistance = TotalRollDistance * EasedAlpha;
	float PrevDistance = TotalRollDistance * EasedPrevAlpha;
	float MoveDistance = CurrentDistance - PrevDistance;
	MoveDistance = FMath::Max(0.0f, MoveDistance); // 음수 방지

	FVector Forward = RollCharacter->GetActorForwardVector();
	// 충돌 처리 가능하도록 Sweep true 유지
	RollCharacter->AddActorWorldOffset(Forward * MoveDistance, true);

	if (Alpha >= 1.0f)
	{
		RollCharacter->GetWorldTimerManager().ClearTimer(RollTimerHandle);
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}