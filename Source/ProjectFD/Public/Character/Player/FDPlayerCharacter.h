// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "Character/FDCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/FDGameplayStatic.h"
#include "Interfaces/InventoryInterface.h"
#include "Interfaces/WeaponInterface.h"
#include "FDPlayerCharacter.generated.h"

class UNiagaraComponent;
class UCharacterJumpAbility;
class UNiagaraSystem;
struct FFDEquipmentEntry;

class USphereComponent;
class AFDPreviewCharacter;
struct FOnAttributeChangeData;
class UAttributeSet;
class UEquipmentManagerComponent;
class UInventoryComponent;
class AFDPlayerState;
class UDA_FDGameplayAbilities;
class UPlayerAbilitySystemComponent;
class UGameplayEffect;
struct FGameplayTag;
class UPlayerCombatComponent;
class UDA_FDInput;
enum class EFDAbilityInputID : uint8;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class USpringArmComponent;
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDetectedActor, AActor*, DetectedActor);

UCLASS()
class PROJECTFD_API AFDPlayerCharacter : public AFDCharacter, public IInventoryInterface, public IWeaponInterface
{
	GENERATED_BODY()

public:
	AFDPlayerCharacter();
	
	virtual void BeginPlay() override;
	
	virtual UInventoryComponent* GetInventoryComponent_Implementation() override;
	virtual void SetInventoryComponent(TArray<FFDInventoryEntry> InEntries) override;
	virtual AEquipmentActor* GetWeaponActor() override;

	UFUNCTION()
	virtual void SetWeaponActor(AEquipmentActor* WeaponActor) override;

	UEquipmentManagerComponent* GetEquipmentComponent();
	
	void FireWeapon(bool IsFire);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void NetFireWeapon(bool IsFire);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerFireWeapon(bool IsFire);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

#pragma region Inventory

private:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Replicated)
	TObjectPtr<UInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Replicated)
	TObjectPtr<UEquipmentManagerComponent> EquipmentComponent;

	UFUNCTION(Client, Unreliable)
	void RefreshPreviewCharacter();

	UFUNCTION(Client, Unreliable)
	void RefreshEquippedWeaponSlot(int SlotNum);

	UFUNCTION(Client, Unreliable)
	void RefreshEquippedModuleSlot(int SlotNum);

	UFUNCTION(Client, Unreliable)
	void RefreshModuleEffectInfo();

	UFUNCTION(Client, Unreliable)
	void RefreshHudWeaponInfo(FFDEquipmentEntry UnEquippedEntry);
	
	void BindCallbacksToDependencies();

	// UPROPERTY(Replicated)
	TSubclassOf<UAnimInstance> CachedAnimInstance;

#pragma endregion

#pragma region HUD

public:
	UFUNCTION(Client, Reliable)
	void NotifySkillCoolDown(FGameplayTag CooldownTag);

#pragma endregion
	
#pragma region Camera

public:
	UPROPERTY(EditDefaultsOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
	USpringArmComponent* CameraBoom;
private:
	
	UPROPERTY(EditDefaultsOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
	UCameraComponent* FollowCam;

	UFUNCTION()
	void OnBunnySkillTwoTagUpdate(const FGameplayTag Tag, int32 NewCount);
	
#pragma endregion
	
#pragma region Input

public:
	
	AFDPreviewCharacter* GetPreviewCharacter();

protected:
	// 클라이언트 재시작시 호출되는 함수 오버라이드
	virtual void PawnClientRestart() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;


private:
	
	UPROPERTY(EditDefaultsOnly, Category= "DataControl", meta = (AllowPrivateAccess = "true"))
	UDA_FDInput* InputData;
	
	void LookAction(const FInputActionValue& InputActionValue);
	void MoveAction(const FInputActionValue& InputActionValue);
	void SprintAction();
	void InventoryAction();
	void AbilityInput(const FInputActionValue& InputActionValue, EFDAbilityInputID AbilityInputID);

	UPROPERTY()
	UDA_FDGameplayAbilities* AbilitiesData;

	UPROPERTY()
	AFDPreviewCharacter* PreviewCharacter = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Class")
	TSubclassOf<AFDPreviewCharacter> PreviewCharacterClass;
	
	FVector GetLookRightDirection();
	FVector GetLookForwardDirection();
	FVector GetMoveForwardDirection();
	
public:
	FORCEINLINE bool GetSprinting() const { return bIsSprint; }
	FORCEINLINE bool GetJumping() const { return GetCharacterMovement()->IsFalling(); }

#pragma endregion
	
#pragma region GAS
public:
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Game Ability System
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
	void CacheAbilitySystemComponent();
	
#pragma endregion

#pragma region Sprint

private:
	UPROPERTY(ReplicatedUsing = OnRep_Sprint)
	bool bIsSprint;

	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	float SprintSpeed;
	float CurrentMaxWalkSpeed;

	UFUNCTION()
	void OnRep_Sprint();

	UFUNCTION(Server, Reliable)
	void ServerSetSprinting(bool IsSprinting);

	UFUNCTION()
	void ChangeSprintSpeed(bool IsSprinting);
	
#pragma endregion

#pragma region Combat

private:
	// UPROPERTY(ReplicatedUsing = OnRep_Combat)
	// TEnumAsByte<ECombatState> CombatState;

	UFUNCTION()
	void OnRep_ActiveWeapon();

	// UPROPERTY(Replicated)
	UPROPERTY(ReplicatedUsing = OnRep_ActiveWeapon)
	AEquipmentActor* ActiveWeapon = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TSubclassOf<UAnimInstance> CombatAnimInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat", meta=(AllowPrivateAccess="true"))
	UPlayerCombatComponent* CombatComponent;

public:
	FORCEINLINE UPlayerCombatComponent* GetCombatComponent() const { return CombatComponent; }

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "GE")
	TSubclassOf<UGameplayEffect> GrantTagGEClass;

	// 자동회복 게임이펙트
	UPROPERTY(EditDefaultsOnly, Category= "GE")
	TSubclassOf<UGameplayEffect> RecoveryShieldEffect;

	void RecoveryShield();

	void OnShieldAttributeChanged(const FOnAttributeChangeData& Data);
	
	FActiveGameplayEffectHandle RecoveryShieldEffectHandle;

	UFUNCTION()
	void OnCombatTagUpdate(const FGameplayTag Tag, int32 NewCount);

	FGameplayTag GetActiveWeaponAmmoType();
	UFUNCTION(BlueprintCallable)
	void ChangeWeaponBySlot(int SlotNum);
	
#pragma endregion

#pragma region DoubleJump
	virtual void Landed(const FHitResult& Hit) override;

	UFUNCTION(Server, Reliable)
	void Server_RequestLanding(const FVector& StartLocation, const FVector& ImpactLocation, float Radius, float Damage, UNiagaraSystem* NiagaraSystem);

#pragma endregion

#pragma region Death,Respawn,DBNO

	void BindGASChangeDelegate();
	void StartDeathSequence();
	void Respawn();
	void DeathTagUpdate(const FGameplayTag Tag, int32 NewCount);
	void DBNOTagUpdate(const FGameplayTag Tag, int32 NewCount);

	UPROPERTY(EditDefaultsOnly, Category="Death", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeathMontage;
	UPROPERTY(EditDefaultsOnly, Category="Death", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* RevivalMontage;
	
	void PlayDeathMontage();
	void PlayRevivalMontage();

	virtual void OnDead();
	virtual void OnRespawn();

	bool CharacterInvincible = false;

#pragma endregion

#pragma region Attachment
	
public:
	UFUNCTION(Server, Reliable)
	void Server_AttachToBossSocket(AActor* BossActor, FName BoneName);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_AttachToBossSocket(AActor* BossActor, FName BoneName);

	UFUNCTION(Server, Reliable)
	void Server_DetachFromBossSocket();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_DetachFromBossSocket();

private:
	void DoAttachToBossSocket(AActor* BossActor, FName BoneName);
	void DoDetachFromBossSocket();
	
#pragma endregion

#pragma region Detection
	
public:
	// DBNORescue 등에 전달할 감지된 액터 델리게이트
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FOnDetectedActor OnDetectedActor;
	
protected:
	// 트리거 볼륨
	UPROPERTY(VisibleAnywhere, Category="Interaction")
	USphereComponent* InteractionTrigger;

	// 트레이스 설정
	UPROPERTY(EditDefaultsOnly, Category="Interaction")
	float TraceDistance = 600.f;

	UPROPERTY(EditDefaultsOnly, Category="Interaction")
	float TraceInterval = 0.5f;

private:
	FTimerHandle TraceTimerHandle;
	int32 OverlapCount = 0;
	bool bTraceEnabled = false;

	// 오버랩 이벤트
	UFUNCTION()
	void OnInteractionBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
							UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
							const FHitResult& SweepResult);

	UFUNCTION()
	void OnInteractionEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
						  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void StartTrace();
	void StopTrace();
	void TraceTick();

	// FDPlayerCharacter만 검출
	AActor* LineTraceForActor(FHitResult& OutHit) const;

public:
	UFUNCTION(Client, Reliable)
	void ClientNotifyTargetDetected(AActor* DetectedActor);

#pragma endregion

#pragma region RotationControl
public:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetUseControllerRotationYaw(bool bNewValue);

#pragma endregion

#pragma region Niagara, Sound

public:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SpawnSkillNiagaraEffectAtLocation(UNiagaraSystem* NiagaraSystem, const FVector& OutStart, const FVector& OutEnd);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SpawnSkillNiagaraEffectAttachedToComponent(UNiagaraSystem* NiagaraSystem, FName SocketName);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_DestroySkillNiagara();

	UPROPERTY(Transient)
	UNiagaraComponent* ActiveSkillTwoNiagara = nullptr;
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlaySkillSoundAtLocation(USoundBase* SkillSound, const FVector& Location);

	UPROPERTY(EditDefaultsOnly, Category="Effects")
	UNiagaraSystem* ShieldDamagedNiagaraEffect;
	
	UPROPERTY(EditDefaultsOnly, Category="Effects")
	UNiagaraSystem* ShieldBreakNiagaraEffect;
	
#pragma endregion
};
