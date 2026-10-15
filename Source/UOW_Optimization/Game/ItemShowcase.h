#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemShowcase.generated.h"

class UItemDataAsset;
class UStaticMeshComponent;
class UTextRenderComponent;
class UPointLightComponent;
class UMaterialInstanceDynamic;

UCLASS()
class UOW_OPTIMIZATION_API AItemShowcase : public AActor
{
	GENERATED_BODY()

public:
	AItemShowcase();

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual bool ShouldTickIfViewportsOnly() const override { return true; }

protected:
	void ClearDisplays();
	void PopulateFromAssetRegistry();
	void BuildDisplay(UItemDataAsset* Item);

	UPROPERTY(EditAnywhere, Category = "Showcase")
	TArray<TObjectPtr<UItemDataAsset>> Items;

	UPROPERTY(EditAnywhere, Category = "Showcase")
	bool bAutoPopulate = true;

	UPROPERTY(EditAnywhere, Category = "Showcase", meta = (EditCondition = "bAutoPopulate"))
	FString AutoPopulatePath = TEXT("/Game/UOW_Custom");

	UPROPERTY(EditAnywhere, Category = "Showcase", meta = (ClampMin = 1))
	int32 Columns = 10;

	UPROPERTY(EditAnywhere, Category = "Showcase", meta = (ClampMin = 0))
	float Spacing = 300.f;

	UPROPERTY(EditAnywhere, Category = "Showcase", meta = (ClampMin = 1))
	float DisplaySize = 120.f;

	UPROPERTY(EditAnywhere, Category = "Showcase")
	float RotationSpeed = 45.f;

	UPROPERTY(EditAnywhere, Category = "Showcase")
	bool bShowBounds = true;

	UPROPERTY()
	TArray<TObjectPtr<UItemDataAsset>> DisplayedItems;

	UPROPERTY()
	TArray<TObjectPtr<UStaticMeshComponent>> DisplayMeshes;

	UPROPERTY()
	TArray<TObjectPtr<UTextRenderComponent>> Labels;

	UPROPERTY()
	TArray<TObjectPtr<UPointLightComponent>> Lights;

	UPROPERTY()
	TArray<TObjectPtr<UMaterialInstanceDynamic>> DisplayMaterials;

	float RunningTime = 0.f;
};
