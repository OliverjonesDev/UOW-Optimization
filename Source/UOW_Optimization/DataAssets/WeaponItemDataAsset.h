#pragma once

#include "CoreMinimal.h"
#include "DataAssets/ItemDataAsset.h"
#include "WeaponItemDataAsset.generated.h"

class UAnimMontage;

USTRUCT(BlueprintType)
struct FWeaponAttack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", meta = (ClampMin = 0))
	float DamageMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", meta = (ClampMin = 0))
	float StaminaCost = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", meta = (ClampMin = 0))
	float ManaCost = 0.f;
};

UCLASS(BlueprintType)
class UOW_OPTIMIZATION_API UWeaponItemDataAsset : public UItemDataAsset
{
	GENERATED_BODY()

public:
	virtual EItemType GetItemType() const override { return EItemType::Weapon; }

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (ClampMin = 0))
	float Damage = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (ClampMin = 0))
	float Range = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Combo", meta = (TitleProperty = "Montage"))
	TArray<FWeaponAttack> LightAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Combo")
	FWeaponAttack HeavyAttack;
};
