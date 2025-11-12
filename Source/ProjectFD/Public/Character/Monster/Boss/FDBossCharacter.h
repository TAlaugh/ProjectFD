

#pragma once

#include "CoreMinimal.h"
#include "Character/Monster/FDMonsterCharacter.h"
#include "FDBossCharacter.generated.h"

class UNiagaraComponent;
class AFDBossParts;
class UBoxComponent;
class AFDMonsterCharacter;

DECLARE_DELEGATE_OneParam(FOnTargetInteractedDelegate, AActor*)

USTRUCT(BlueprintType)
struct FBossPart
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss Parts")
	float PartHealth = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss Parts")
	float MaxPartHealth = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss Parts")
	FName BoneName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss Parts")
	FName AttachBoneName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss Parts")
	FGameplayTag DestroyTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss Parts")
	bool bOnlyDamageInBerserk = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss Parts")
	bool bCanAttach = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnBossStatChanged, float, CurrentHP, float, MaxHP, float, CurrentShield, float, MaxShield);

/**
 * 
 */
UCLASS()
class PROJECTFD_API AFDBossCharacter : public AFDMonsterCharacter
{
	GENERATED_BODY()
	
public:
	AFDBossCharacter();
	
	// Delegate
	FOnTargetInteractedDelegate OnCollisionHitTarget;
	FOnTargetInteractedDelegate OnCollisionPulledFromTarget;
	FOnBossStatChanged OnBossStatChanged;

	// Getter
	UBoxComponent* GetGuillotineCollision() const { return BoxCollision; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetBerserkMode() { return bIsBerserk; }

	// Replication
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void DropBossItems();
	
protected:
	// LifeCycle
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	// Collision handling
	UFUNCTION()
	virtual void OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// BerserkMode
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> IncreaseBerserkGageGE;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> BerserkGE;

	UPROPERTY()
	FActiveGameplayEffectHandle ActiveIncreaseBerserkGage;

	UPROPERTY()
	FActiveGameplayEffectHandle ActiveBerserkGE;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated)
	bool bIsBerserk = false;

	UFUNCTION()
	void IncreaseBerserkGage();

	UFUNCTION()
	void StartBerserk();

	UFUNCTION()
	void EndBerserk();
	
	FTimerHandle BerserkTimerHandle;

public:
	// Boss Parts
	UFUNCTION()
	void DestroyPart(const FName& BoneName);

	UFUNCTION(NetMulticast, Unreliable)
	void NetMulticast_TwinklePart(const FName& BoneName);

	UFUNCTION(NetMulticast, Unreliable)
	void NetMulticast_TwinklePart_Warning(const FName& BoneName);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_HideBossHUD();

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_OnBerserk_NS();
	
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_OffBerserk_NS();
	
	UFUNCTION()
	void DestroyEndBerserk();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<USkeletalMeshComponent*> Parts;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	TArray<FBossPart> BossPartsInfo;

	UPROPERTY()
	int32 ExecutionCall_LoopCount = 0;

	UPROPERTY()
	FVector ExecutionCall_DamageVector;
	
protected:
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UNiagaraComponent* BerserkNS;
	
	// Boss Parts Component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss Parts")
	USkeletalMeshComponent* HeadShell_L;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss Parts")
	USkeletalMeshComponent* HeadShell_R;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss Parts")
	USkeletalMeshComponent* Clav_L;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss Parts")
	USkeletalMeshComponent* Clav_R;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss Parts")
	USkeletalMeshComponent* RealCore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss Parts")
	USkeletalMeshComponent* CoreShell;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss Parts")
	USkeletalMeshComponent* Knee_L;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss Parts")
	USkeletalMeshComponent* Knee_R;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss Parts")
	USkeletalMeshComponent* Plasma;

	// Collision
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Collision")
	UBoxComponent* BoxCollision;

	
};
