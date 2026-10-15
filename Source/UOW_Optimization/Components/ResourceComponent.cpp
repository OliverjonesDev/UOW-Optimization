#include "Components/ResourceComponent.h"

UResourceComponent::UResourceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UResourceComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentValue = MaxValue;
	OnValueChanged.Broadcast(CurrentValue, MaxValue);
}

void UResourceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (RegenDelayRemaining > 0.f)
	{
		RegenDelayRemaining -= DeltaTime;
	}
	else
	{
		SetCurrentValue(CurrentValue + RegenRate * DeltaTime);
	}

	UE_LOG(LogTemp, Log, TEXT("%s %s: %s / %s"), *GetOwner()->GetName(), *GetName(), *FString::SanitizeFloat(CurrentValue), *FString::SanitizeFloat(MaxValue));
}

bool UResourceComponent::Consume(float Amount)
{
	if (Amount <= 0.f || !HasEnough(Amount))
	{
		return false;
	}

	RegenDelayRemaining = RegenDelay;
	SetCurrentValue(CurrentValue - Amount);
	return true;
}

void UResourceComponent::Restore(float Amount)
{
	if (Amount > 0.f)
	{
		SetCurrentValue(CurrentValue + Amount);
	}
}

void UResourceComponent::SetMaxValue(float NewMax, bool bRefill)
{
	MaxValue = FMath::Max(0.f, NewMax);
	SetCurrentValue(bRefill ? MaxValue : CurrentValue);
}

float UResourceComponent::GetPercent() const
{
	return MaxValue > 0.f ? CurrentValue / MaxValue : 0.f;
}

void UResourceComponent::SetCurrentValue(float NewValue)
{
	CurrentValue = FMath::Clamp(NewValue, 0.f, MaxValue);
	OnValueChanged.Broadcast(CurrentValue, MaxValue);

	if (CurrentValue <= 0.f)
	{
		OnDepleted.Broadcast();
	}
}
