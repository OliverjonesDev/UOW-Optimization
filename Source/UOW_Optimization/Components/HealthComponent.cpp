#include "Components/HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	RegenRate = 0.f;
	RegenDelay = 5.f;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeAnyDamage);
	}
}

void UHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (bIsDead || Damage <= 0.f)
	{
		return;
	}

	RegenDelayRemaining = RegenDelay;
	SetCurrentValue(CurrentValue - Damage);

	if (IsEmpty())
	{
		bIsDead = true;
		OnDeath.Broadcast(InstigatedBy);
	}
}
