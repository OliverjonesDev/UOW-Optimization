#pragma once

#include "CoreMinimal.h"
#include "Components/ResourceComponent.h"
#include "StaminaComponent.generated.h"

UCLASS(ClassGroup = (Attributes), meta = (BlueprintSpawnableComponent))
class UOW_OPTIMIZATION_API UStaminaComponent : public UResourceComponent
{
	GENERATED_BODY()

public:
	UStaminaComponent();

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void StartDrain(float RatePerSecond);

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void StopDrain();

	UFUNCTION(BlueprintPure, Category = "Stamina")
	bool IsDraining() const { return DrainRate > 0.f; }

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	float DrainRate = 0.f;
};
