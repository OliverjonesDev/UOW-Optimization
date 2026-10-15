#pragma once

#include "CoreMinimal.h"
#include "DataAssets/ItemDataAsset.h"
#include "PotionItemDataAsset.generated.h"

UCLASS(BlueprintType)
class UOW_OPTIMIZATION_API UPotionItemDataAsset : public UItemDataAsset
{
	GENERATED_BODY()

public:
	virtual EItemType GetItemType() const override { return EItemType::Potion; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Potion", meta = (ClampMin = 0))
	float HealthRestore = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Potion", meta = (ClampMin = 0))
	float StaminaRestore = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Potion", meta = (ClampMin = 0))
	float ManaRestore = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Potion", meta = (ClampMin = 0))
	float Duration = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Potion", meta = (ClampMin = 1))
	int32 MaxStackSize = 10;
};
