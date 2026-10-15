#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataAssets/ItemDataAsset.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

UCLASS(ClassGroup = (Inventory), meta = (BlueprintSpawnableComponent))
class UOW_OPTIMIZATION_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddItem(UItemDataAsset* Item);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(UItemDataAsset* Item);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool HasItem(const UItemDataAsset* Item) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<UItemDataAsset*> GetItems() const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<UItemDataAsset*> GetItemsOfType(EItemType Type) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetTotalGoldValue() const;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChanged OnInventoryChanged;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<TObjectPtr<UItemDataAsset>> Items;
};
