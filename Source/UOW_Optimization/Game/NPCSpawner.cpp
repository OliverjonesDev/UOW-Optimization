#include "Game/NPCSpawner.h"
#include "Characters/UOW_OptimizationCharacter.h"
#include "Components/InventoryComponent.h"
#include "Components/HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ANPCSpawner::ANPCSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void ANPCSpawner::BeginPlay()
{
	Super::BeginPlay();

	for (int32 Index = 0; Index < Population; Index++)
	{
		SpawnCharacter();
	}

	GetWorldTimerManager().SetTimer(DamageTimer, this, &ANPCSpawner::DamageRandomCharacter, DamageInterval, true);
}

void ANPCSpawner::SpawnCharacter()
{
	if (!CharacterClass)
	{
		return;
	}

	const FVector2D Offset = FMath::RandPointInCircle(SpawnRadius);
	const FVector Location = GetActorLocation() + FVector(Offset.X, Offset.Y, 0.f);
	const FRotator Rotation(0.f, FMath::FRandRange(0.f, 360.f), 0.f);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AUOW_OptimizationCharacter* Character = GetWorld()->SpawnActor<AUOW_OptimizationCharacter>(CharacterClass, Location, Rotation, SpawnParams);
	if (!Character)
	{
		return;
	}

	Character->SpawnDefaultController();

	if (!ItemPool.IsEmpty())
	{
		for (int32 Index = 0; Index < ItemsPerCharacter; Index++)
		{
			Character->GetInventoryComponent()->AddItem(ItemPool[FMath::RandRange(0, ItemPool.Num() - 1)]);
		}
	}

	Character->GetHealthComponent()->OnDeath.AddDynamic(this, &ANPCSpawner::HandleCharacterDeath);
	SpawnedCharacters.Add(Character);
}

void ANPCSpawner::DamageRandomCharacter()
{
	if (SpawnedCharacters.IsEmpty())
	{
		return;
	}

	AUOW_OptimizationCharacter* Target = SpawnedCharacters[FMath::RandRange(0, SpawnedCharacters.Num() - 1)];
	if (Target)
	{
		UGameplayStatics::ApplyDamage(Target, DamageAmount, nullptr, this, UDamageType::StaticClass());
	}
}

void ANPCSpawner::HandleCharacterDeath(AController* Killer)
{
	SpawnCharacter();
}
