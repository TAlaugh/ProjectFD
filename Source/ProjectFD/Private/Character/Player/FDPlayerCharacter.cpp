// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/FDPlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/Combat/Player/PlayerCombatComponent.h"
#include "DataAssets/StartUpData/Player/DA_FDInput.h"
#include "Gameframework/FDPlayerState.h"
#include "GAS/FDGameplayAbilityTypes.h"
#include "GAS/FDGameplayStatic.h"
#include "GameplayEffectTypes.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/Player/FDPreviewCharacter.h"
#include "Components/SphereComponent.h"
#include "Controller/FDPlayerController.h"
#include "Engine/OverlapResult.h"
#include "Equipment/EquipmentActor.h"
#include "Gameframework/FDGameState.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/SpectatorPawnMovement.h"
#include "GAS/GA/Player/CharacterJumpAbility.h"
#include "GAS/Player/PlayerAttributeSet.h"
#include "GAS/Player/PlayerAbilitySystemComponent.h"
#include "Inventory/InventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Widget/WidgetController/InventoryWidgetController.h"
#include "Widget/WidgetController/PlayerHudWidgetController.h"


AFDPlayerCharacter::AFDPlayerCharacter()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->bUsePawnControlRotation = true;

	FollowCam = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCam"));
	FollowCam->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	bUseControllerRotationYaw = false;

	// 캐릭터가 이동하는 방향으로 회전하도록 설정
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 225.f, 0.f);

	SprintSpeed = 800.f;

	// CombatComponent 세팅
	CombatComponent = CreateDefaultSubobject<UPlayerCombatComponent>(TEXT("PlayerCombatComponent"));

	TeamID = FGenericTeamId(0); // TeamID 초기화

	//////////////////////// Inventory Moving //////////////////////////////

	// mayday
	bReplicates = true;

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("InventoryComponent");
	InventoryComponent->SetIsReplicated(true);

	EquipmentComponent = CreateDefaultSubobject<UEquipmentManagerComponent>("EquipmentComponent");
	EquipmentComponent->SetIsReplicated(true);

	
	// 상호작용 트리거 박스 생성
	InteractionTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionTrigger"));
	InteractionTrigger->SetupAttachment(GetRootComponent());
	InteractionTrigger->SetSphereRadius(250.f);
	InteractionTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionTrigger->SetCollisionObjectType(ECC_WorldDynamic);
	InteractionTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	InteractionTrigger->SetGenerateOverlapEvents(true);
	InteractionTrigger->OnComponentBeginOverlap.AddDynamic(this, &AFDPlayerCharacter::OnInteractionBegin);
	InteractionTrigger->OnComponentEndOverlap.AddDynamic(this, &AFDPlayerCharacter::OnInteractionEnd);
}

void AFDPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	BindCallbacksToDependencies();
	
	// Disable Inputs
	DisableInput(GetController<APlayerController>());

	// // Restore Inventory After Spanwning
	// if (HasAuthority())
	// {
	// 	if (AFDPlayerState* PS = GetPlayerState<AFDPlayerState>())
	// 	{
	// 		PS->RestoreInventories(this);
	// 	}
	// 	if (InventoryComponent->GetInventoryEntries().IsEmpty() || EquipmentComponent->EquipmentList.GetWeaponEntries().IsEmpty())
	// 	{
	// 		FItemAcquisitionInfo NewItemInfo;
	// 		NewItemInfo.MinLevel = 1;
	// 		NewItemInfo.MaxLevel = 10;
	// 		InventoryComponent->AddItem(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon.MG.EnduringLegacy"), NewItemInfo, 1);
	// 		InventoryComponent->AddItem(FGameplayTag::RequestGameplayTag("Item.Equipment.Module.Player.SkillInsight"), NewItemInfo, 1);
	// 	}
	// }

	CurrentMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
}

UInventoryComponent* AFDPlayerCharacter::GetInventoryComponent_Implementation()
{
	return InventoryComponent;
}

void AFDPlayerCharacter::SetInventoryComponent(TArray<FFDInventoryEntry> InEntries)
{
}

AEquipmentActor* AFDPlayerCharacter::GetWeaponActor()
{
	if (ActiveWeapon)
	{
		return ActiveWeapon;
	}

	return nullptr;
}

void AFDPlayerCharacter::SetWeaponActor(AEquipmentActor* WeaponActor)
{
	// 바인딩
	if (ActiveWeapon)
	{
		ActiveWeapon->SetOwner(nullptr);
		ActiveWeapon->WeaponFireDelegate.Broadcast(false);
		ActiveWeapon->WeaponReloadDelegate.Broadcast(false);
	}
	
	ActiveWeapon = WeaponActor;
	
	if (ActiveWeapon)
	{
		ActiveWeapon->SetOwner(this);
	}
}

UEquipmentManagerComponent* AFDPlayerCharacter::GetEquipmentComponent()
{
	return EquipmentComponent;
}

void AFDPlayerCharacter::NetFireWeapon_Implementation(bool IsFire)
{
	if (ActiveWeapon)
	{
		ActiveWeapon->WeaponFireDelegate.Broadcast(IsFire);
	}
}

void AFDPlayerCharacter::FireWeapon(bool IsFire)
{
	if (HasAuthority())
	{
		if (ActiveWeapon)
		{
			ActiveWeapon->WeaponFireDelegate.Broadcast(IsFire);
		}
		NetFireWeapon(IsFire);
	}
}

void AFDPlayerCharacter::ServerFireWeapon_Implementation(bool IsFire)
{
	if (ActiveWeapon)
	{
		ActiveWeapon->WeaponFireDelegate.Broadcast(IsFire);
	}
	NetFireWeapon(IsFire);
}

void AFDPlayerCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(AFDPlayerCharacter, bIsSprint, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME(AFDPlayerCharacter, InventoryComponent);
	DOREPLIFETIME(AFDPlayerCharacter, EquipmentComponent);
	DOREPLIFETIME(AFDPlayerCharacter, ActiveWeapon);
	// DOREPLIFETIME(AFDPlayerCharacter, CachedAnimInstance);
}

void AFDPlayerCharacter::RefreshModuleEffectInfo_Implementation()
{
	// Cast<AFDPlayerController>(GetLocalViewingPlayerController())->GetInventoryWidgetController()->BroadcastPlayerModuleInventoryInitialValues();
	Cast<AFDPlayerController>(GetLocalViewingPlayerController())->RefreshModuleEffectSlot();
}

void AFDPlayerCharacter::RefreshPreviewCharacter_Implementation()
{
	if (PreviewCharacter)
	{
		PreviewCharacter->EquipPreviewWeapon(ActiveWeapon);
		PreviewCharacter->ChangeAnimLayer(CachedAnimInstance);
		// FTimerHandle TimerHandle;
		// GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		// {
		// 	PreviewCharacter->ChangeAnimLayer(CachedAnimInstance);
		// },
		// 0.1f,
		// false);
	}
}

void AFDPlayerCharacter::RefreshEquippedWeaponSlot_Implementation(int SlotNum)
{
	Cast<AFDPlayerController>(GetLocalViewingPlayerController())->RefreshUnEquippedWeaponSlot(SlotNum);
}

void AFDPlayerCharacter::RefreshHudWeaponInfo_Implementation(FFDEquipmentEntry UnEquippedEntry)
{
	Cast<AFDPlayerController>(GetLocalViewingPlayerController())->CallWeaponSlotToDefault(UnEquippedEntry);
}

void AFDPlayerCharacter::RefreshEquippedModuleSlot_Implementation(int SlotNum)
{
	Cast<AFDPlayerController>(GetLocalViewingPlayerController())->RefreshUnEquippedModuleSlot(SlotNum);
}

void AFDPlayerCharacter::BindCallbacksToDependencies()
{
	if (IsValid(InventoryComponent) && IsValid(EquipmentComponent))
	{
		InventoryComponent->EquipmentItemDelegate.AddLambda(
			[this] (const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition, const FEquipmentEffectPackage& EffectPackage, int SlotNum, FExtraEquipmentInfo ItemInfo)
			{
				if (IsValid(EquipmentComponent))
				{
					EquipmentComponent->EquipItem(EquipmentDefinition, EffectPackage, SlotNum, ItemInfo);
				}
			});
		InventoryComponent->ModuleItemDelegate.AddLambda(
			[this] (const TSubclassOf<UModuleDefinition>& ModuleDefinition, const FEquipmentEffectPackage& EffectPackage, int SlotNum, FExtraEquipmentInfo ItemInfo)
			{
				if (IsValid(EquipmentComponent))
				{
					EquipmentComponent->EquipModule(ModuleDefinition, EffectPackage, SlotNum, ItemInfo);
				}
			});
		EquipmentComponent->EquipmentList.EquipmentEntryDelegate.AddLambda(
			[this] (const FFDEquipmentEntry& Entry)
			{
				// GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("Is Active Weapon??"), true);
				if (Entry.bIsActiveWeapon)
				{
					// GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("Changing AnimLayer!!"), true);
					GetMesh()->GetAnimInstance()->LinkAnimClassLayers(Entry.GetCharacterAnimLayer());
					CachedAnimInstance = Entry.GetCharacterAnimLayer();
					if (PreviewCharacter)
					{
						RefreshPreviewCharacter();
						PreviewCharacter->ChangeAnimLayer(Entry.GetCharacterAnimLayer());
					}
					// Entry.OnEquip(this, true);
				}
			});
		EquipmentComponent->EquipmentList.UnEquipmentEntryDelegate.AddLambda(
			[this] (const FFDEquipmentEntry& UnEquippedEntry)
			{
				if (IsValid(InventoryComponent))
				{
					if (UnEquippedEntry.EntryTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon")))
					{
						InventoryComponent->AddUnEquippedItemEntry(UnEquippedEntry, UnEquippedEntry.EffectPackage);
						if (HasAuthority())
						{
							RefreshEquippedWeaponSlot(UnEquippedEntry.SlotNum);
							RefreshHudWeaponInfo(UnEquippedEntry);
						}
					}
					else if (UnEquippedEntry.EntryTag.MatchesTag(FGameplayTag::RequestGameplayTag("Item.Equipment.Module")))
					{
						InventoryComponent->AddUnEquippedModuleEntry(UnEquippedEntry, UnEquippedEntry.EffectPackage);
						if (HasAuthority())
						{
							RefreshEquippedModuleSlot(UnEquippedEntry.SlotNum);
							RefreshModuleEffectInfo();
						}
					}
				}
			});
	}
}

void AFDPlayerCharacter::NotifySkillCoolDown_Implementation(FGameplayTag CooldownTag)
{
	Cast<AFDPlayerController>(GetLocalViewingPlayerController())->StartSkillCoolDownByTag(CooldownTag);
}

// 전투 맵에서 IMC 바꾸도록 설정
void AFDPlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
	APlayerController* OwningPlayerController = GetController<APlayerController>();
	if (OwningPlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* InputSubsystem = OwningPlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if (InputSubsystem)
		{
			// 재접속시 기존의 IMC를 제거하고 새로 등록한다.
			InputSubsystem->RemoveMappingContext(InputData->BasicInputMappingContext);
			InputSubsystem->AddMappingContext(InputData->BasicInputMappingContext, 0);
		}
	}
}

void AFDPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(InputData->IA_Look, ETriggerEvent::Triggered, this, &AFDPlayerCharacter::LookAction);
		EnhancedInputComponent->BindAction(InputData->IA_Move, ETriggerEvent::Triggered, this, &AFDPlayerCharacter::MoveAction);
		EnhancedInputComponent->BindAction(InputData->IA_Sprint, ETriggerEvent::Started, this, &AFDPlayerCharacter::SprintAction);
		// // GA_RangeWeaponAttack에 대한 입력 바인딩 추가 ( InputPressed와 InputReleased 이벤트를 사용하기 위해 )
		EnhancedInputComponent->BindAction(InputData->IA_Fire, ETriggerEvent::Triggered, this, &AFDPlayerCharacter::AbilityInput, EFDAbilityInputID::RangeAttack);
		EnhancedInputComponent->BindAction(InputData->IA_Fire, ETriggerEvent::Completed, this, &AFDPlayerCharacter::AbilityInput, EFDAbilityInputID::RangeAttack);
		EnhancedInputComponent->BindAction(InputData->IA_WireAction, ETriggerEvent::Triggered, this, &AFDPlayerCharacter::AbilityInput, EFDAbilityInputID::WireAction);
		EnhancedInputComponent->BindAction(InputData->IA_WireAction, ETriggerEvent::Completed, this, &AFDPlayerCharacter::AbilityInput, EFDAbilityInputID::WireAction);
		EnhancedInputComponent->BindAction(InputData->IA_HangOnGrab, ETriggerEvent::Triggered, this, &AFDPlayerCharacter::AbilityInput, EFDAbilityInputID::HangOnGrab);
		EnhancedInputComponent->BindAction(InputData->IA_HangOnGrab, ETriggerEvent::Completed, this, &AFDPlayerCharacter::AbilityInput, EFDAbilityInputID::HangOnGrab);
		EnhancedInputComponent->BindAction(InputData->IA_Rescue, ETriggerEvent::Triggered, this, &AFDPlayerCharacter::AbilityInput, EFDAbilityInputID::Rescue);
		EnhancedInputComponent->BindAction(InputData->IA_Rescue, ETriggerEvent::Completed, this, &AFDPlayerCharacter::AbilityInput, EFDAbilityInputID::Rescue);
		EnhancedInputComponent->BindAction(InputData->IA_Aim, ETriggerEvent::Triggered, this, &AFDPlayerCharacter::AbilityInput, EFDAbilityInputID::Aim);
		EnhancedInputComponent->BindAction(InputData->IA_Aim, ETriggerEvent::Completed, this, &AFDPlayerCharacter::AbilityInput, EFDAbilityInputID::Aim);
		// Open Inventory
		EnhancedInputComponent->BindAction(InputData->IA_Inventory, ETriggerEvent::Triggered, this, &AFDPlayerCharacter::InventoryAction);

		for (const TPair<EFDAbilityInputID, UInputAction*>& InputActionPair : InputData->GameplayAbilityInputActions)
		{
			EnhancedInputComponent->BindAction(InputActionPair.Value, ETriggerEvent::Triggered, this, &AFDPlayerCharacter::AbilityInput, InputActionPair.Key);
		}
	}
}

void AFDPlayerCharacter::OnBunnySkillTwoTagUpdate(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0)
	{
		CameraBoom->TargetArmLength += 200.f;
	}
	else
	{
		CameraBoom->TargetArmLength -= 200.f;
	}
}

AFDPreviewCharacter* AFDPlayerCharacter::GetPreviewCharacter()
{
	if (PreviewCharacter)
	{
		return PreviewCharacter;
	}

	return nullptr;
}

void AFDPlayerCharacter::LookAction(const FInputActionValue& InputActionValue)
{
	FVector2D InputValue = InputActionValue.Get<FVector2D>();

	AddControllerPitchInput(-InputValue.Y);
	AddControllerYawInput(InputValue.X);
}

void AFDPlayerCharacter::MoveAction(const FInputActionValue& InputActionValue)
{
	FVector2D InputValue = InputActionValue.Get<FVector2D>();

	AddMovementInput(GetMoveForwardDirection()*InputValue.Y + GetLookRightDirection()*InputValue.X);
}

void AFDPlayerCharacter::SprintAction()
{
	bIsSprint = !bIsSprint;
	ServerSetSprinting(bIsSprint);
}

void AFDPlayerCharacter::InventoryAction()
{
	if (not PreviewCharacter)
	{
		FVector SpawnLocation(0.f, 0.f, -1000.f);
		FRotator SpawnRotation(0.f, 0.f, 0.f);

		// 소환 파라미터
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		PreviewCharacter = GetWorld()->SpawnActor<AFDPreviewCharacter>(PreviewCharacterClass, SpawnLocation, SpawnRotation, SpawnParams);

		RefreshPreviewCharacter();
	}
	
	RefreshPreviewCharacter();
	
	Cast<AFDPlayerController>(GetLocalViewingPlayerController())->CreateInventoryWidget();
}

void AFDPlayerCharacter::AbilityInput(const FInputActionValue& InputActionValue, EFDAbilityInputID AbilityInputID)
{
	if (!ASC)
	{
		UE_LOG(LogTemp, Error, TEXT("ASC가 nullptr입니다! AbilityInputID: %d"), (int32)AbilityInputID);
		return;
	}
	bool bPressed = InputActionValue.Get<bool>();
	//UE_LOG(LogTemp, Warning, TEXT("AbilityInput called: %d, Pressed: %d"), (int32)AbilityInputID, bPressed);
	if (bPressed)
	{
		ASC->AbilityLocalInputPressed((int32)AbilityInputID);
	}
	else
	{
		ASC->AbilityLocalInputReleased((int32)AbilityInputID);
	}
}

FVector AFDPlayerCharacter::GetLookRightDirection()
{
	return FollowCam->GetRightVector();
}

FVector AFDPlayerCharacter::GetLookForwardDirection()
{
	return FollowCam->GetForwardVector();
}

FVector AFDPlayerCharacter::GetMoveForwardDirection()
{
	// 카메라의 오른쪽 벡터와 월드의 위쪽 벡터의 외적을 구하면 카메라의 앞쪽 벡터를 얻을 수 있다. ( 오른쪽 방향과 위쪽 방향의 수직인 방향이 전방 방향이기 때문 )
	return FVector::CrossProduct(GetLookRightDirection(), FVector::UpVector);
}


UAbilitySystemComponent* AFDPlayerCharacter::GetAbilitySystemComponent() const
{
	return Super::GetAbilitySystemComponent();
}

void AFDPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (HasAuthority())
	{
		CacheAbilitySystemComponent();
		if (AFDPlayerState* PS = GetPlayerState<AFDPlayerState>())
		{
			ASC->InitAbilityActorInfo(PS, this);
			BindGASChangeDelegate();
			PS->AddStartupAbilities();
		}

		FTimerHandle InventoryTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(InventoryTimerHandle, [this]()
		{
			// Restore Inventory After Spawning
			if (AFDPlayerState* PS = GetPlayerState<AFDPlayerState>())
			{
				PS->RestoreInventories(this);
			}
			if (InventoryComponent->GetInventoryEntries().IsEmpty() && EquipmentComponent->EquipmentList.GetWeaponEntries().IsEmpty())
			{
				FItemAcquisitionInfo NewItemInfo;
				NewItemInfo.MinLevel = 1;
				NewItemInfo.MaxLevel = 5;
				InventoryComponent->AddItem(FGameplayTag::RequestGameplayTag("Item.Equipment.Weapon.MG.EnduringLegacy"), NewItemInfo, 1);
				InventoryComponent->AddItem(FGameplayTag::RequestGameplayTag("Item.Equipment.Module.Player.SkillInsight"), NewItemInfo, 1);
			}
		},
		3.f,
		false);
	}
}

void AFDPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	CacheAbilitySystemComponent();
	if (AFDPlayerState* PS = GetPlayerState<AFDPlayerState>())
	{
		ASC->InitAbilityActorInfo(PS, this);
		BindGASChangeDelegate();
	}
}

void AFDPlayerCharacter::CacheAbilitySystemComponent()
{
	if (AFDPlayerState* PS = GetPlayerState<AFDPlayerState>())
	{
		ASC = PS->GetAbilitySystemComponent();
	}
}

void AFDPlayerCharacter::OnRep_Sprint()
{
	ChangeSprintSpeed(bIsSprint);
}

void AFDPlayerCharacter::ChangeSprintSpeed(bool IsSprinting)
{
	if (ASC->HasMatchingGameplayTag(UFDGameplayStatic::GetDBNOStatusTag())) return;
	if (ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Ability.BunnySkill.Two")))) return;
	if (IsSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = CurrentMaxWalkSpeed;
	}
}

void AFDPlayerCharacter::OnRep_ActiveWeapon()
{
	if (PreviewCharacter)
	{
		PreviewCharacter->EquipPreviewWeapon(ActiveWeapon);
	}
}

float AFDPlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                     AActor* DamageCauser)
{
	if (CharacterInvincible) return -1.f;
	const float Clamped = -DamageAmount;

	// 서버에서만 처리(ApplyDamage는 서버에서 호출되는 것이 정석)
	if (!HasAuthority())
	{
		return Clamped;
	}
	
	// GE 스펙 생성 및 적용
	FGameplayEffectContextHandle GEContextHandle = ASC->MakeEffectContext();
	FGameplayEffectSpecHandle GrantTagSpecHandle = ASC->MakeOutgoingSpec(GrantTagGEClass, 1.f, GEContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*GrantTagSpecHandle.Data.Get());

	// 속성에 데미지 적용
	const UFDAttributeSet* AttributeSet = ASC->GetSet<UFDAttributeSet>();
	float CurrentShield = AttributeSet->GetShield();
	FGameplayAttribute CurrentHealthAttribute = UPlayerAttributeSet::GetHealthAttribute();
	FGameplayAttribute CurrentShieldAttribute = UPlayerAttributeSet::GetShieldAttribute();

	FVector AttackerLoc = (DamageCauser ? DamageCauser->GetActorLocation() : GetActorLocation() - GetActorForwardVector() * 100.f);
	
	if ( CurrentShield >= -Clamped && CurrentShield > 0 )
	{
		ASC->ApplyModToAttributeUnsafe(CurrentShieldAttribute, EGameplayModOp::Additive, Clamped);
		Multicast_SpawnSkillNiagaraEffectAtLocation(ShieldDamagedNiagaraEffect,  GetActorLocation() + FVector(0,0,50), AttackerLoc);
	}
	else if ( -Clamped > CurrentShield && CurrentShield > 0 )
	{
		ASC->ApplyModToAttributeUnsafe(CurrentShieldAttribute, EGameplayModOp::Additive, -CurrentShield);
		ASC->ApplyModToAttributeUnsafe(CurrentHealthAttribute, EGameplayModOp::Additive, Clamped + CurrentShield);
		Multicast_SpawnSkillNiagaraEffectAtLocation(ShieldBreakNiagaraEffect, GetActorLocation(), GetActorLocation());
	}
	else if ( CurrentShield <= 0 )
	{
		ASC->ApplyModToAttributeUnsafe(CurrentHealthAttribute, EGameplayModOp::Additive, Clamped);
	}
	
	return Clamped;
}

void AFDPlayerCharacter::RecoveryShield()
{

	const UFDAttributeSet* AttributeSet = ASC->GetSet<UFDAttributeSet>();
	float CurrentShield = AttributeSet->GetShield();
	float MaxShield = AttributeSet->GetMaxShield();
	if (CurrentShield < MaxShield)
	{
		// 중복 적용 방지
		if (!RecoveryShieldEffectHandle.IsValid() && RecoveryShieldEffect)
		{
			FGameplayEffectContextHandle Ctx = ASC->MakeEffectContext();
			FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(RecoveryShieldEffect, 1.f, Ctx);
			if (Spec.IsValid())
			{
				RecoveryShieldEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
			}
		}
	}
	else // CurrentShield >= MaxShield
	{
		if (RecoveryShieldEffectHandle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(RecoveryShieldEffectHandle);
			RecoveryShieldEffectHandle.Invalidate();
		}
	}
}

void AFDPlayerCharacter::OnShieldAttributeChanged(const FOnAttributeChangeData& Data)
{
	RecoveryShield();
}

void AFDPlayerCharacter::OnCombatTagUpdate(const FGameplayTag Tag, int32 NewCount)
{
	const bool bInCombat = (NewCount > 0);

	// 전투 중에는 컨트롤러 Yaw로 회전, 비전투는 이동 방향으로 회전
	bUseControllerRotationYaw = bInCombat;
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->bOrientRotationToMovement = !bInCombat;
		// 필요 시 회전 속도 조정
		// MoveComp->RotationRate = FRotator(0.f, bInCombat ? 720.f : 360.f, 0.f);
	}
}

FGameplayTag AFDPlayerCharacter::GetActiveWeaponAmmoType()
{
	return EquipmentComponent->GetActiveWeaponAmmoType();
}

void AFDPlayerCharacter::ChangeWeaponBySlot(int SlotNum)
{
	EquipmentComponent->ChangeWeapon(SlotNum);
}

void AFDPlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	UE_LOG(LogTemp, Warning, TEXT("Landed called, resetting JumpCount"));
}

void AFDPlayerCharacter::Server_RequestLanding_Implementation(const FVector& StartLocation,
	const FVector& ImpactLocation, float Radius, float Damage, UNiagaraSystem* NiagaraSystem)
{
	// 서버에서만 실행
	if (!HasAuthority()) return;

	// AbilitySystemComponent 캐시 체크
	if (!ASC) CacheAbilitySystemComponent();
	if (!ASC) return;

	const UPlayerAttributeSet* PlayerAttrSet = ASC->GetSet<UPlayerAttributeSet>();
	if (!PlayerAttrSet) return;

	// Sweep (Sphere sweep) 기반 히트 검색으로 변경
	TArray<FHitResult> HitResults;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius);
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Server_RequestLanding), false, this);
	Params.bTraceComplex = false;
	// Pawn 채널 대상으로 검색
	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_Pawn);

	// Sweep은 start와 end가 필요하므로 아주 작은 거리로 설정하여 사실상 그 지점의 스피어 충돌을 검사
	const FVector SweepStart = ImpactLocation + FVector(0.0f, 0.0f, 0.01f);
	const FVector SweepEnd = ImpactLocation - FVector(0.0f, 0.0f, 0.01f);

	IGenericTeamAgentInterface* OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(this);
	
	bool bAny = GetWorld()->SweepMultiByObjectType(HitResults, SweepStart, SweepEnd, FQuat::Identity, ObjParams, Sphere, Params);
	if (bAny)
	{
		for (FHitResult& Hit : HitResults)
		{
			Hit.bBlockingHit = false;
			AActor* HitActor = Hit.GetActor();
			if (!HitActor || HitActor == this) continue;

			if (OwnerTeamInterface)
			{
				const ETeamAttitude::Type OtherActorTeamAttitude = OwnerTeamInterface->GetTeamAttitudeTowards(*HitActor);
				if (OtherActorTeamAttitude != ETeamAttitude::Hostile)
				{
					continue;
				}
			}

			// 치명타 계산
			bool bIsCritical = FMath::FRand() <= PlayerAttrSet->GetSkillCriticalChance();
			float AppliedDamage = Damage;
			if (bIsCritical)
			{
				AppliedDamage *= PlayerAttrSet->GetSkillCriticalDamage();
			}

			// ApplyPointDamage에 실제 HitResult 사용
			UGameplayStatics::ApplyPointDamage(HitActor, AppliedDamage, (HitActor->GetActorLocation() - StartLocation).GetSafeNormal(), Hit, GetController(), this, UDamageType::StaticClass());
		}
	}

	// 이펙트 멀티캐스트
	Multicast_SpawnSkillNiagaraEffectAtLocation(NiagaraSystem, ImpactLocation, ImpactLocation);
}

void AFDPlayerCharacter::BindGASChangeDelegate()
{
	if (ASC)
	{
		// 태그 등록 및 소멸시 타는 델리게이트
		ASC->RegisterGameplayTagEvent(UFDGameplayStatic::GetDeadStatusTag()).AddUObject(this, &AFDPlayerCharacter::DeathTagUpdate);
		ASC->RegisterGameplayTagEvent(UFDGameplayStatic::GetDBNOStatusTag()).AddUObject(this, &AFDPlayerCharacter::DBNOTagUpdate);
		
		// Shield 속성 변경을 감지하는 델리게이트 바인딩
		const UPlayerAttributeSet* PlayerAttributeSet = Cast<UPlayerAttributeSet>(ASC->GetAttributeSet(UPlayerAttributeSet::StaticClass()));
		if (PlayerAttributeSet)
		{
			ASC->GetGameplayAttributeValueChangeDelegate(PlayerAttributeSet->GetShieldAttribute()).AddUObject(this, &AFDPlayerCharacter::OnShieldAttributeChanged);
		}

		// Combat 태그 변화 바인딩 및 즉시 초기화
		const FGameplayTag CombatTag = UFDGameplayStatic::GetOwnerCombatTag();
		ASC->RegisterGameplayTagEvent(CombatTag)
		   .AddUObject(this, &AFDPlayerCharacter::OnCombatTagUpdate);

		const int32 CurrentCount = ASC->HasMatchingGameplayTag(CombatTag) ? 1 : 0;
		OnCombatTagUpdate(CombatTag, CurrentCount);

		ASC->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("Ability.BunnySkill.Two")))
			.AddUObject(this, &AFDPlayerCharacter::OnBunnySkillTwoTagUpdate);
		
	}
}

void AFDPlayerCharacter::StartDeathSequence()
{
	CharacterInvincible = true;
	OnDead();
	PlayDeathMontage();
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AFDPlayerCharacter::Respawn()
{
	CharacterInvincible = true;
	UE_LOG(LogTemp, Warning, TEXT("Respawn"));
	OnRespawn();
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->GetAnimInstance()->StopAllMontages(0.f);
	ChangeSprintSpeed(bIsSprint);
	PlayRevivalMontage();

	if (ASC)
	{
		if (UPlayerAbilitySystemComponent* PASC = Cast<UPlayerAbilitySystemComponent>(ASC))
		{
			PASC->ApplyFullStatEffect();
		}
	}
	
	// 서버에서만 플레이어의 PlayerStart를 찾아 텔레포트
	if (HasAuthority())
	{
		if (AController* PC = GetController())
		{
			if (AGameModeBase* GM = GetWorld()->GetAuthGameMode())
			{
				if (AActor* StartActor = GM->FindPlayerStart(PC))
				{
					const FVector SpawnLocation = StartActor->GetActorLocation();
					const FRotator SpawnRotation = StartActor->GetActorRotation();
					// 충돌 무시하여 즉시 이동
					TeleportTo(SpawnLocation, SpawnRotation, false, true);
					FTimerHandle InvincobleHandle;

					GetWorldTimerManager().SetTimer(InvincobleHandle, [this]()
					{
						CharacterInvincible = false;
					}, 2.f, false);
				}
			}
		}
	}
}

void AFDPlayerCharacter::DeathTagUpdate(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount != 0) // New
	{
		StartDeathSequence();
	}
	else
	{
		if (AFDGameState* GS = GetWorld()->GetGameState<AFDGameState>())
		{
			if (GS->SharedRespawns > 0 )
			{
				Respawn();
			}
			else
			{
				// 기존 폰 파괴
				Destroy();
			}
		}
	}
}

void AFDPlayerCharacter::DBNOTagUpdate(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount != 0)
	{
		// DBNO 상태 진입 시 스프린트 속도 초기화
		ChangeSprintSpeed(false);
	}
	else
	{
		ChangeSprintSpeed(true);
	}
}

void AFDPlayerCharacter::PlayDeathMontage()
{
	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
}

void AFDPlayerCharacter::PlayRevivalMontage()

{
	if (RevivalMontage)
	{
		PlayAnimMontage(RevivalMontage);
	}
}

void AFDPlayerCharacter::OnDead()
{
	APlayerController* PC = GetController<APlayerController>();
	if (PC)
	{
		DisableInput(PC);
	}
}

void AFDPlayerCharacter::OnRespawn()
{
	APlayerController* PC = GetController<APlayerController>();
	if (PC)
	{
		EnableInput(PC);
	}
}


void AFDPlayerCharacter::Server_DetachFromBossSocket_Implementation()
{
	Multicast_DetachFromBossSocket();
}

void AFDPlayerCharacter::Multicast_DetachFromBossSocket_Implementation()
{
	DoDetachFromBossSocket();
}

void AFDPlayerCharacter::DoAttachToBossSocket(AActor* BossActor, FName BoneName)
{
	if (!BossActor) return;
	if (GetWeaponActor())
	{
		GetWeaponActor()->SetWeaponVisibility(false);
	}
	if (USkeletalMeshComponent* Skel = BossActor->FindComponentByClass<USkeletalMeshComponent>())
	{
		if (!BoneName.IsNone() && Skel->DoesSocketExist(BoneName))
		{
			if (UCapsuleComponent* Capsule = GetCapsuleComponent())
			{
				bUseControllerRotationYaw = false;
				if (UCharacterMovementComponent* Move = GetCharacterMovement())
				{
					Move->SetMovementMode(MOVE_None);
					Move->GravityScale = 0.f;
				}
				Capsule->AttachToComponent(Skel, FAttachmentTransformRules::SnapToTargetIncludingScale, BoneName);
				if (AController* PC = GetController())
				{
					//PC->SetControlRotation(GetActorRotation());
					// 카메라 회전 막기
					if (CameraBoom)
					{
						CameraBoom->bUsePawnControlRotation = false;
					}
				}
			}
		}
	}
}

void AFDPlayerCharacter::DoDetachFromBossSocket()
{
	if (GetWeaponActor())
	{
		GetWeaponActor()->SetWeaponVisibility(true);
	}
	
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		bUseControllerRotationYaw = true;
		if (UCharacterMovementComponent* Move = GetCharacterMovement())
		{
			Move->SetMovementMode(MOVE_Walking);
			Move->GravityScale = 1.f;
		}
		Capsule->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		Capsule->SetRelativeRotation(FRotator::ZeroRotator);
		// 카메라 회전 풀기
		if (CameraBoom)
		{
			CameraBoom->bUsePawnControlRotation = true;
		}
	}
}

void AFDPlayerCharacter::OnInteractionBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;  
	if (!OtherActor || OtherActor == this) return;

	++OverlapCount;
	if (!bTraceEnabled)
	{
		StartTrace();
	}
}

void AFDPlayerCharacter::OnInteractionEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!HasAuthority()) return;  
	if (!OtherActor || OtherActor == this) return;

	OverlapCount = FMath::Max(0, OverlapCount - 1);
	if (OverlapCount == 0)
	{
		StopTrace();
	}
}

void AFDPlayerCharacter::StartTrace()
{
	if (!HasAuthority()) return;  
	if (bTraceEnabled) return;
	bTraceEnabled = true;

	GetWorldTimerManager().SetTimer(
		TraceTimerHandle, this, &AFDPlayerCharacter::TraceTick, TraceInterval, true);
}

void AFDPlayerCharacter::StopTrace()
{
	if (!HasAuthority()) return;  
	if (!bTraceEnabled) return;
	bTraceEnabled = false;

	GetWorldTimerManager().ClearTimer(TraceTimerHandle);
}

void AFDPlayerCharacter::TraceTick()
{
	if (!HasAuthority()) return;  
	FHitResult Hit;
	if (LineTraceForActor(Hit))
	{
		if (AActor* HitActor = Hit.GetActor())
		{
			// FDPlayerCharacter를 맞춘 경우에만 반환(브로드캐스트)
			OnDetectedActor.Broadcast(HitActor);
			//UE_LOG(LogTemp, Warning, TEXT("%s -> Detected : %s"),*GetName() ,*Hit.GetActor()->GetName());
		}
	}
}

AActor* AFDPlayerCharacter::LineTraceForActor(FHitResult& OutHit) const
{
	const FVector Start = FollowCam ? FollowCam->GetComponentLocation()
									: GetActorLocation() + FVector(0.f, 0.f, BaseEyeHeight);
	const FRotator ViewRot = GetControlRotation();
	const FVector End = Start + ViewRot.Vector() * TraceDistance;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(InteractionTrace), false, this);
	Params.bTraceComplex = false;
	Params.AddIgnoredActor(this);

	// Pawn 채널만 대상으로 트레이스
	const FCollisionObjectQueryParams ObjParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects);
	// 가급적 Pawn만
	FCollisionObjectQueryParams PawnOnly;
	PawnOnly.AddObjectTypesToQuery(ECC_Pawn);

	//bool bHit =
	GetWorld()->LineTraceSingleByObjectType(OutHit, Start, End, PawnOnly, Params);

// #if !(UE_BUILD_SHIPPING)
// 	// 디버그 표시 \- 필요 시 주석 처리
// 	const FColor C = bHit ? FColor::Green : FColor::Red;
// 	DrawDebugLine(GetWorld(), Start, End, C, false, TraceInterval * 1.1f, 0, 0.5f);
// #endif
	
	
	return Cast<AActor>(OutHit.GetActor());
}

void AFDPlayerCharacter::Multicast_SpawnSkillNiagaraEffectAttachedToComponent_Implementation(
	UNiagaraSystem* NiagaraSystem, FName SocketName)
{
	USceneComponent* AttachTo = GetMesh();
	FRotator NiagaraRoation = FRotator::ZeroRotator;

	if (!GetMesh()->DoesSocketExist(SocketName))
	{
		if (ActiveWeapon)
		{
			if (USkeletalMeshComponent* WeaponMesh = ActiveWeapon->FindComponentByClass<USkeletalMeshComponent>())
			{
				AttachTo = WeaponMesh;
				NiagaraRoation = FRotator::ZeroRotator + FRotator(0.f, -90.f, 0.f);
			}
		}
	}
	UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
	NiagaraSystem,
	AttachTo,
	SocketName,
	FVector::ZeroVector,
	NiagaraRoation,
	EAttachLocation::SnapToTarget,
	true,
	true
	);
	
	if (NiagaraComp)
	{
		NiagaraComp->ComponentTags.Add(FName("SkillTwo")); // 파괴용 식별 태그
		ActiveSkillTwoNiagara = NiagaraComp;
	}
}

void AFDPlayerCharacter::Multicast_DestroySkillNiagara_Implementation()
{
	if (ActiveSkillTwoNiagara)
	{
		ActiveSkillTwoNiagara->Deactivate();
		ActiveSkillTwoNiagara->DestroyComponent();
		ActiveSkillTwoNiagara = nullptr;
		return;
	}

	if (USkeletalMeshComponent* LocalMesh = GetMesh())
	{
		TArray<USceneComponent*> ChildComponents;
		LocalMesh->GetChildrenComponents(true, ChildComponents);
		for (USceneComponent* ChildComp : ChildComponents)
		{
			if (ChildComp && ChildComp->ComponentTags.Contains(FName("SkillTwo")))
			{
				if (UNiagaraComponent* NC = Cast<UNiagaraComponent>(ChildComp))
				{
					NC->Deactivate();
					NC->DestroyComponent();
				}
				else
				{
					ChildComp->DestroyComponent();
				}
			}
		}
	}
}

void AFDPlayerCharacter::Multicast_PlaySkillSoundAtLocation_Implementation(USoundBase* SkillSound,
                                                                           const FVector& Location)
{
	if (!SkillSound)
	{
		return;
	}
	UGameplayStatics::PlaySoundAtLocation(this, SkillSound, Location);
}

void AFDPlayerCharacter::Multicast_SpawnSkillNiagaraEffectAtLocation_Implementation(UNiagaraSystem* NiagaraSystem,
                                                                                    const FVector& OutStart, const FVector& OutEnd)
{
	const FRotator NiagaraRotator = (OutEnd - OutStart).Rotation();
	UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		NiagaraSystem,
		OutStart,
		NiagaraRotator,
		FVector(1.f),
		true,
		true
		);

	if (NiagaraComp)
	{
		NiagaraComp->SetVariableVec3(FName("User.BeamEnd"), OutEnd);
	}
}

void AFDPlayerCharacter::MulticastSetUseControllerRotationYaw_Implementation(bool bNewValue)
{
	bUseControllerRotationYaw = bNewValue;
}

void AFDPlayerCharacter::ClientNotifyTargetDetected_Implementation(AActor* DetectedActor)
{
	// 클라에서 동일 델리게이트 브로드캐스트 → 능력이 로컬에서도 HandleDetectedActor 수신
	OnDetectedActor.Broadcast(DetectedActor);
}

void AFDPlayerCharacter::Multicast_AttachToBossSocket_Implementation(AActor* BossActor, FName BoneName)
{
	DoAttachToBossSocket(BossActor, BoneName);
}

void AFDPlayerCharacter::Server_AttachToBossSocket_Implementation(AActor* BossActor, FName BoneName)
{
	Multicast_AttachToBossSocket(BossActor, BoneName);
}

void AFDPlayerCharacter::ServerSetSprinting_Implementation(bool IsSprinting)
{
	// 서버에서 클라값으로 갱신하면서 속도 변경
	bIsSprint = IsSprinting;
	ChangeSprintSpeed(bIsSprint);
}
