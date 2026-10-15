#pragma once

#include "CoreMinimal.h"
#include "Components/ResourceComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath, AController*, Killer);

UCLASS(ClassGroup = (Attributes), meta = (BlueprintSpawnableComponent))
class UOW_OPTIMIZATION_API UHealthComponent : public UResourceComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const { return bIsDead; }

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDeath OnDeath;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	bool bIsDead = false;
};
