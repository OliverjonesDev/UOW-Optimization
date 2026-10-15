#pragma once

#include "CoreMinimal.h"
#include "DataAssets/ItemDataAsset.h"
#include "IngredientItemDataAsset.generated.h"

class UPotionItemDataAsset;

UCLASS(BlueprintType)
class UOW_OPTIMIZATION_API UIngredientItemDataAsset : public UItemDataAsset
{
	GENERATED_BODY()

public:
	virtual EItemType GetItemType() const override { return EItemType::Ingredient; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ingredient", meta = (ClampMin = 1))
	int32 MaxStackSize = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ingredient", meta = (ClampMin = 0))
	float Potency = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ingredient")
	TArray<TObjectPtr<UPotionItemDataAsset>> UsedInPotions;
};
