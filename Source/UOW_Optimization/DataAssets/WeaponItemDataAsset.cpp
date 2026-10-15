#include "DataAssets/WeaponItemDataAsset.h"
#include "Misc/DataValidation.h"

#if WITH_EDITOR
EDataValidationResult UWeaponItemDataAsset::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (LightAttacks.Num() < 2)
	{
		Context.AddError(FText::FromString(TEXT("Light combo needs at least 2 attacks.")));
		Result = EDataValidationResult::Invalid;
	}

	for (int32 Index = 0; Index < LightAttacks.Num(); Index++)
	{
		if (!LightAttacks[Index].Montage)
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Light attack %d has no montage."), Index)));
			Result = EDataValidationResult::Invalid;
		}
	}

	if (!HeavyAttack.Montage)
	{
		Context.AddError(FText::FromString(TEXT("Heavy attack has no montage.")));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif
