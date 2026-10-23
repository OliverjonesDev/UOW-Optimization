#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemDataAsset.generated.h"

class UNiagaraSystem;
class UTexture2D;
class UStaticMesh;

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Misc,
	Weapon,
	Potion,
	Ingredient
};

// Make inherit from UItemInterface for optimizations in the future. 

UCLASS(Abstract, BlueprintType)
class UOW_OPTIMIZATION_API UItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (ClampMin = 0))
	int32 GoldCost = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
	TObjectPtr<UStaticMesh> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
	TObjectPtr<UNiagaraSystem> ParticleSystem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TArray<TObjectPtr<UItemDataAsset>> RelatedItems;

	UFUNCTION(BlueprintPure, Category = "Item")
	virtual EItemType GetItemType() const PURE_VIRTUAL(UItemDataAsset::GetItemType, return EItemType::Misc;);

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
