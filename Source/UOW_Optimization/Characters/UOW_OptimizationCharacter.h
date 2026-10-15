#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "Logging/LogMacros.h"
#include "UOW_OptimizationCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UInventoryComponent;
class UHealthComponent;
class UStaminaComponent;
class UManaComponent;
class UCombatComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractionTargetFound, AActor*, Target, FText, Prompt);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionTargetLost, AActor*, PreviousTarget);

UCLASS(config=Game)
class UOW_OPTIMIZATION_API AUOW_OptimizationCharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	UInventoryComponent* InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	UStaminaComponent* StaminaComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	UManaComponent* ManaComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	UCombatComponent* CombatComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LightAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* HeavyAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;

public:
	AUOW_OptimizationCharacter();

	virtual void Tick(float DeltaSeconds) override;
	virtual void PossessedBy(AController* NewController) override;

	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId(TeamId); }

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void StartSprint();

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void StopSprint();

	UFUNCTION(BlueprintPure, Category = "Actions")
	bool IsSprinting() const { return bIsSprinting; }

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void Interact();

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void InteractWith(AActor* Target);

	UFUNCTION(BlueprintPure, Category = "Actions")
	AActor* GetInteractionTarget() const { return InteractionTarget; }

	UFUNCTION(BlueprintPure, Category = "Actions")
	bool CanInteract() const { return bHasInteractionTarget; }

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractionTargetFound OnInteractionTargetFound;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractionTargetLost OnInteractionTargetLost;

	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	FORCEINLINE UHealthComponent* GetHealthComponent() const { return HealthComponent; }
	FORCEINLINE UStaminaComponent* GetStaminaComponent() const { return StaminaComponent; }
	FORCEINLINE UManaComponent* GetManaComponent() const { return ManaComponent; }
	FORCEINLINE UCombatComponent* GetCombatComponent() const { return CombatComponent; }

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	UFUNCTION()
	void HandleDeath(AController* Killer);

	AActor* FindNearestCharacter() const;
	void UpdateInventoryStats();
	void ConfigureForPlayer();
	void RefreshPlayerStats();
	void EnsureInputMappings();
	void UpdateInteractionTarget();
	void DropLoot();
	void SetInteractionTarget(AActor* NewTarget);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float WalkSpeed = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float SprintSpeed = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float SprintStaminaDrain = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	uint8 TeamId = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "AI")
	TObjectPtr<AActor> NearestCharacter;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Inventory")
	int32 InventoryGoldValue = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Inventory")
	int32 WeaponCount = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Inventory")
	int32 PotionCount = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Inventory")
	int32 IngredientCount = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Inventory")
	int32 MiscCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	float InteractionDistance = 50000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (ClampMin = 0))
	float InteractionRadius = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (ClampMin = 0))
	float InteractionHalfHeight = 100.f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<AActor> InteractionTarget;

	bool bHasInteractionTarget = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	float StatsRefreshRate = 0.001f;

	TArray<FString> MovementHistory;
	FTimerHandle StatsTimer;
	bool bIsSprinting = false;
};
