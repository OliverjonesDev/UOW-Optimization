#include "Components/StaminaComponent.h"

UStaminaComponent::UStaminaComponent()
{
	RegenRate = 20.f;
	RegenDelay = 1.f;
}

void UStaminaComponent::StartDrain(float RatePerSecond)
{
	DrainRate = FMath::Max(0.f, RatePerSecond);
}

void UStaminaComponent::StopDrain()
{
	DrainRate = 0.f;
	RegenDelayRemaining = RegenDelay;
}

void UStaminaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (IsDraining())
	{
		RegenDelayRemaining = RegenDelay;
		SetCurrentValue(CurrentValue - DrainRate * DeltaTime);
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
