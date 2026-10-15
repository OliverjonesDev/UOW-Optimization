#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ResourceComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnResourceChanged, float, NewValue, float, NewMax);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnResourceDepleted);

UCLASS(Abstract, ClassGroup = (Attributes))
class UOW_OPTIMIZATION_API UResourceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UResourceComponent();

	UFUNCTION(BlueprintCallable, Category = "Resource")
	bool Consume(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Resource")
	void Restore(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Resource")
	void SetMaxValue(float NewMax, bool bRefill);

	UFUNCTION(BlueprintPure, Category = "Resource")
	float GetCurrentValue() const { return CurrentValue; }

	UFUNCTION(BlueprintPure, Category = "Resource")
	float GetMaxValue() const { return MaxValue; }

	UFUNCTION(BlueprintPure, Category = "Resource")
	float GetPercent() const;

	UFUNCTION(BlueprintPure, Category = "Resource")
	bool HasEnough(float Amount) const { return CurrentValue >= Amount; }

	UFUNCTION(BlueprintPure, Category = "Resource")
	bool IsEmpty() const { return CurrentValue <= 0.f; }

	UPROPERTY(BlueprintAssignable, Category = "Resource")
	FOnResourceChanged OnValueChanged;

	UPROPERTY(BlueprintAssignable, Category = "Resource")
	FOnResourceDepleted OnDepleted;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

	void SetCurrentValue(float NewValue);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resource", meta = (ClampMin = 0))
	float MaxValue = 100.f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Resource")
	float CurrentValue = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resource", meta = (ClampMin = 0))
	float RegenRate = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resource", meta = (ClampMin = 0))
	float RegenDelay = 1.f;

	float RegenDelayRemaining = 0.f;
};
