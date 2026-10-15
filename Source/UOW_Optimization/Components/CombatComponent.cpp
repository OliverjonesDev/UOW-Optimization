#include "Components/CombatComponent.h"
#include "Components/HealthComponent.h"
#include "Components/StaminaComponent.h"
#include "Components/ManaComponent.h"
#include "DataAssets/WeaponItemDataAsset.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "HAL/IConsoleManager.h"

static bool GCombatDebugDrawTraces = false;
static FAutoConsoleVariableRef CVarCombatDebugDrawTraces(
	TEXT("Combat.Debug.DrawTraces"),
	GCombatDebugDrawTraces,
	TEXT("Draw combat hit traces while an attack hit window is active."),
	ECVF_Cheat);

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	HealthComponent = GetOwner()->FindComponentByClass<UHealthComponent>();
	StaminaComponent = GetOwner()->FindComponentByClass<UStaminaComponent>();
	ManaComponent = GetOwner()->FindComponentByClass<UManaComponent>();
}

void UCombatComponent::EquipWeapon(UWeaponItemDataAsset* Weapon)
{
	if (bIsAttacking)
	{
		return;
	}

	EquippedWeapon = Weapon;
	ResetCombo();
}

void UCombatComponent::LightAttack()
{
	if (!EquippedWeapon || EquippedWeapon->LightAttacks.IsEmpty())
	{
		return;
	}

	if (!bIsAttacking)
	{
		StartLightAttack(0);
		return;
	}

	if (bIsHeavy)
	{
		return;
	}

	if (bComboWindowOpen)
	{
		StartLightAttack(ComboIndex + 1);
	}
	else
	{
		bComboQueued = true;
	}
}

void UCombatComponent::HeavyAttack()
{
	if (!EquippedWeapon || bIsAttacking)
	{
		return;
	}

	if (StartAttack(EquippedWeapon->HeavyAttack))
	{
		bIsHeavy = true;
		ComboIndex = INDEX_NONE;
		OnAttackStarted.Broadcast(true, ComboIndex);
	}
}

void UCombatComponent::StartLightAttack(int32 Index)
{
	if (!EquippedWeapon->LightAttacks.IsValidIndex(Index))
	{
		return;
	}

	if (StartAttack(EquippedWeapon->LightAttacks[Index]))
	{
		bIsHeavy = false;
		ComboIndex = Index;
		OnAttackStarted.Broadcast(false, ComboIndex);
	}
}

bool UCombatComponent::StartAttack(const FWeaponAttack& Attack)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character || !Attack.Montage)
	{
		return false;
	}

	if (HealthComponent && HealthComponent->IsDead())
	{
		return false;
	}

	if ((StaminaComponent && !StaminaComponent->HasEnough(Attack.StaminaCost)) || (ManaComponent && !ManaComponent->HasEnough(Attack.ManaCost)))
	{
		return false;
	}

	UAnimInstance* AnimInstance = Character->GetMesh() ? Character->GetMesh()->GetAnimInstance() : nullptr;
	if (!AnimInstance || AnimInstance->Montage_Play(Attack.Montage) <= 0.f)
	{
		return false;
	}

	if (StaminaComponent)
	{
		StaminaComponent->Consume(Attack.StaminaCost);
	}

	if (ManaComponent)
	{
		ManaComponent->Consume(Attack.ManaCost);
	}

	CurrentAttackId++;
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &UCombatComponent::HandleMontageEnded, CurrentAttackId);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, Attack.Montage);

	CurrentDamageMultiplier = Attack.DamageMultiplier;
	HitActors.Reset();
	bHitDetectionActive = false;
	bIsAttacking = true;
	bComboWindowOpen = false;
	bComboQueued = false;
	return true;
}

void UCombatComponent::OpenComboWindow()
{
	if (!bIsAttacking || bIsHeavy)
	{
		return;
	}

	bComboWindowOpen = true;

	if (bComboQueued)
	{
		bComboQueued = false;
		StartLightAttack(ComboIndex + 1);
	}
}

void UCombatComponent::CloseComboWindow()
{
	bComboWindowOpen = false;
}

void UCombatComponent::BeginHitDetection()
{
	if (!bIsAttacking)
	{
		return;
	}

	bHitDetectionActive = true;
	HitActors.Reset();
	PerformHitTrace();
}

void UCombatComponent::EndHitDetection()
{
	bHitDetectionActive = false;
	HitActors.Reset();
}

void UCombatComponent::PerformHitTrace()
{
	AActor* Owner = GetOwner();
	if (!bHitDetectionActive || !bIsAttacking || !EquippedWeapon || !Owner)
	{
		return;
	}

	const FVector Start = Owner->GetActorLocation();
	const FVector End = Start + Owner->GetActorForwardVector() * EquippedWeapon->Range;
	const FCollisionQueryParams Params(SCENE_QUERY_STAT(WeaponHitTrace), false, Owner);

	TArray<FHitResult> Hits;
	GetWorld()->SweepMultiByObjectType(Hits, Start, End, FQuat::Identity, FCollisionObjectQueryParams(ECC_Pawn), FCollisionShape::MakeSphere(HitRadius), Params);

#if ENABLE_DRAW_DEBUG
	if (GCombatDebugDrawTraces)
	{
		const FColor TraceColor = Hits.IsEmpty() ? FColor::Green : FColor::Red;
		const FVector TraceDirection = End - Start;
		const FQuat TraceRotation = FRotationMatrix::MakeFromZ(TraceDirection).ToQuat();
		DrawDebugCapsule(GetWorld(), Start + TraceDirection * 0.5f, TraceDirection.Size() * 0.5f + HitRadius, HitRadius, TraceRotation, TraceColor);

		for (const FHitResult& Hit : Hits)
		{
			DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 10.f, 8, FColor::Yellow);
		}
	}
#endif

	const float Damage = EquippedWeapon->Damage * CurrentDamageMultiplier;
	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor || HitActors.Contains(HitActor))
		{
			continue;
		}

		HitActors.Add(HitActor);
		UGameplayStatics::ApplyDamage(HitActor, Damage, Owner->GetInstigatorController(), Owner, UDamageType::StaticClass());
	}
}

void UCombatComponent::HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted, int32 AttackId)
{
	if (AttackId == CurrentAttackId)
	{
		ResetCombo();
	}
}

void UCombatComponent::ResetCombo()
{
	ComboIndex = INDEX_NONE;
	bIsAttacking = false;
	bIsHeavy = false;
	bComboWindowOpen = false;
	bComboQueued = false;
	bHitDetectionActive = false;
	HitActors.Reset();
}
