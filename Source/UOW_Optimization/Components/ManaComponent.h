#pragma once

#include "CoreMinimal.h"
#include "Components/ResourceComponent.h"
#include "ManaComponent.generated.h"

UCLASS(ClassGroup = (Attributes), meta = (BlueprintSpawnableComponent))
class UOW_OPTIMIZATION_API UManaComponent : public UResourceComponent
{
	GENERATED_BODY()

public:
	UManaComponent();
};
