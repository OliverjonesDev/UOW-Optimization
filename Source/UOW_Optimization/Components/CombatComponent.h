#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class UWeaponItemDataAsset;
class UAnimMontage;
class UHealthComponent;
class UStaminaComponent;
class UManaComponent;
struct FWeaponAttack;

// Very basic combat system - This component handles basic combat functionality such as equipping weapons, performing attacks, and managing combo windows.
// The 

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttackStarted, bool, bHeavy, int32, ComboIndex);

UCLASS(ClassGroup = (Combat), meta = (BlueprintSpawnableComponent))
class UOW_OPTIMIZATION_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void EquipWeapon(UWeaponItemDataAsset* Weapon);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void LightAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void HeavyAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void OpenComboWindow();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void CloseComboWindow();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void BeginHitDetection();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void PerformHitTrace();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void EndHitDetection();

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsAttacking() const { return bIsAttacking; }

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsHeavyAttacking() const { return bIsAttacking && bIsHeavy; }

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsHitDetectionActive() const { return bHitDetectionActive; }

	UFUNCTION(BlueprintPure, Category = "Combat")
	int32 GetComboIndex() const { return ComboIndex; }

	UFUNCTION(BlueprintPure, Category = "Combat")
	UWeaponItemDataAsset* GetEquippedWeapon() const { return EquippedWeapon; }

	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnAttackStarted OnAttackStarted;

protected:
	virtual void BeginPlay() override;

	void StartLightAttack(int32 Index);
	bool StartAttack(const FWeaponAttack& Attack);
	void HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted, int32 AttackId);
	void ResetCombo();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UWeaponItemDataAsset> EquippedWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = 0))
	float HitRadius = 50.f;

	UPROPERTY(Transient)
	TObjectPtr<UHealthComponent> HealthComponent;

	UPROPERTY(Transient)
	TObjectPtr<UStaminaComponent> StaminaComponent;

	UPROPERTY(Transient)
	TObjectPtr<UManaComponent> ManaComponent;

	UPROPERTY(Transient)
	TArray<TObjectPtr<AActor>> HitActors;

	float CurrentDamageMultiplier = 1.f;
	int32 ComboIndex = INDEX_NONE;
	int32 CurrentAttackId = 0;
	bool bIsAttacking = false;
	bool bIsHeavy = false;
	bool bComboWindowOpen = false;
	bool bComboQueued = false;
	bool bHitDetectionActive = false;
};
