#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/Interactable.h"
#include "ItemPickup.generated.h"

class UItemDataAsset;
class UStaticMeshComponent;

UCLASS()
class UOW_OPTIMIZATION_API AItemPickup : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AItemPickup();

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionPrompt_Implementation() const override;

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void SetItem(UItemDataAsset* NewItem, int32 NewQuantity);

	UFUNCTION(BlueprintPure, Category = "Pickup")
	UItemDataAsset* GetItem() const { return Item; }

protected:
	virtual void BeginPlay() override;

	void UpdateMesh();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<UItemDataAsset> Item;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (ClampMin = 1))
	int32 Quantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (ClampMin = 1))
	float DisplaySize = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup|Animation", meta = (ClampMin = 0))
	float BobHeight = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup|Animation", meta = (ClampMin = 0))
	float BobSpeed = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup|Animation")
	float RotationSpeed = 30.f;

	FVector BaseLocation = FVector::ZeroVector;
	FRotator BaseRotation = FRotator::ZeroRotator;
	float RunningTime = 0.f;
	float BobPhase = 0.f;
};
