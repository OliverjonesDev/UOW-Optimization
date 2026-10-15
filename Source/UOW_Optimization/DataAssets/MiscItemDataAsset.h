#pragma once

#include "CoreMinimal.h"
#include "DataAssets/ItemDataAsset.h"
#include "MiscItemDataAsset.generated.h"

UCLASS(BlueprintType)
class UOW_OPTIMIZATION_API UMiscItemDataAsset : public UItemDataAsset
{
	GENERATED_BODY()

public:
	virtual EItemType GetItemType() const override { return EItemType::Misc; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Misc", meta = (ClampMin = 1))
	int32 MaxStackSize = 99;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Misc")
	bool bIsQuestItem = false;
};
