#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NPCSpawner.generated.h"

class AUOW_OptimizationCharacter;
class UItemDataAsset;

UCLASS()
class UOW_OPTIMIZATION_API ANPCSpawner : public AActor
{
	GENERATED_BODY()

public:
	ANPCSpawner();

protected:
	virtual void BeginPlay() override;

	void SpawnCharacter();
	void DamageRandomCharacter();

	UFUNCTION()
	void HandleCharacterDeath(AController* Killer);

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<AUOW_OptimizationCharacter> CharacterClass;

	UPROPERTY(EditAnywhere, Category = "Spawning", meta = (ClampMin = 0))
	int32 Population = 100;

	UPROPERTY(EditAnywhere, Category = "Spawning", meta = (ClampMin = 0))
	float SpawnRadius = 3000.f;

	UPROPERTY(EditAnywhere, Category = "Items")
	TArray<TObjectPtr<UItemDataAsset>> ItemPool;

	UPROPERTY(EditAnywhere, Category = "Items", meta = (ClampMin = 0))
	int32 ItemsPerCharacter = 50;

	UPROPERTY(EditAnywhere, Category = "Damage", meta = (ClampMin = 0.01))
	float DamageInterval = 0.25f;

	UPROPERTY(EditAnywhere, Category = "Damage", meta = (ClampMin = 0))
	float DamageAmount = 40.f;

	UPROPERTY(VisibleInstanceOnly, Category = "Spawning")
	TArray<TObjectPtr<AUOW_OptimizationCharacter>> SpawnedCharacters;

	FTimerHandle DamageTimer;
};
