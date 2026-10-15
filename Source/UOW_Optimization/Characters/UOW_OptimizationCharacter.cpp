#include "Characters/UOW_OptimizationCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/HealthComponent.h"
#include "Components/StaminaComponent.h"
#include "Components/ManaComponent.h"
#include "Components/CombatComponent.h"
#include "Interaction/Interactable.h"
#include "Interaction/ItemPickup.h"
#include "EngineUtils.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include "HAL/IConsoleManager.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

#if ENABLE_DRAW_DEBUG
static bool GDebugDrawInteraction = false;
static FAutoConsoleVariableRef CVarDebugDrawInteraction(
	TEXT("Debug.DrawInteraction"),
	GDebugDrawInteraction,
	TEXT("Draw the player's interaction trace and print what it is hitting."),
	ECVF_Cheat);

static void DrawInteractionDebug(const UWorld* World, const FVector& Start, const FVector& End, const FCollisionShape& Shape, const TArray<FHitResult>& Hits, const AActor* Target)
{
	static constexpr int32 MaxListedHits = 10;

	DrawDebugLine(World, Start, End, Target ? FColor::Green : FColor::Red);

	DrawDebugCapsule(World, Start, Shape.GetCapsuleHalfHeight(), Shape.GetCapsuleRadius(), FQuat::Identity, FColor::Cyan);

	for (const FHitResult& Hit : Hits)
	{
		const FColor HitColor = Hit.GetActor() == Target ? FColor::Green : FColor::Yellow;
		DrawDebugCapsule(World, Hit.Location, Shape.GetCapsuleHalfHeight(), Shape.GetCapsuleRadius(), FQuat::Identity, HitColor);
		DrawDebugPoint(World, Hit.ImpactPoint, 12.f, HitColor);
	}

	if (Target)
	{
		const FBox Bounds = Target->GetComponentsBoundingBox();
		DrawDebugBox(World, Bounds.GetCenter(), Bounds.GetExtent(), FColor::Green);
	}

	if (!GEngine)
	{
		return;
	}

	FString HitList = FString::Printf(TEXT("Interaction hits: %d"), Hits.Num());
	for (int32 Index = 0; Index < Hits.Num() && Index < MaxListedHits; Index++)
	{
		const FHitResult& Hit = Hits[Index];
		HitList += FString::Printf(TEXT("\n  %s (%s) %.1fm"), *GetNameSafe(Hit.GetActor()), *GetNameSafe(Hit.GetComponent()), Hit.Distance / 100.f);
	}

	if (Hits.Num() > MaxListedHits)
	{
		HitList += FString::Printf(TEXT("\n  ...and %d more"), Hits.Num() - MaxListedHits);
	}

	GEngine->AddOnScreenDebugMessage(int32(0x1A7E0001), 0.f, Target ? FColor::Green : FColor::Red, FString::Printf(TEXT("Interaction target: %s"), *GetNameSafe(Target)));
	GEngine->AddOnScreenDebugMessage(int32(0x1A7E0002), 0.f, FColor::Yellow, HitList);
}
#endif

AUOW_OptimizationCharacter::AUOW_OptimizationCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	FollowCamera->PostProcessSettings.bOverride_BloomMethod = true;
	FollowCamera->PostProcessSettings.BloomMethod = BM_FFT;
	FollowCamera->PostProcessSettings.bOverride_DepthOfFieldFocalDistance = true;
	FollowCamera->PostProcessSettings.DepthOfFieldFocalDistance = 400.f;
	FollowCamera->PostProcessSettings.bOverride_DepthOfFieldFstop = true;
	FollowCamera->PostProcessSettings.DepthOfFieldFstop = 1.2f;
	FollowCamera->PostProcessSettings.bOverride_AmbientOcclusionQuality = true;
	FollowCamera->PostProcessSettings.AmbientOcclusionQuality = 100.f;
	FollowCamera->PostProcessSettings.bOverride_AmbientOcclusionRadius = true;
	FollowCamera->PostProcessSettings.AmbientOcclusionRadius = 500.f;

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("StaminaComponent"));
	ManaComponent = CreateDefaultSubobject<UManaComponent>(TEXT("ManaComponent"));
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
}

void AUOW_OptimizationCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	HealthComponent->OnDeath.AddDynamic(this, &AUOW_OptimizationCharacter::HandleDeath);
	StaminaComponent->OnDepleted.AddDynamic(this, &AUOW_OptimizationCharacter::StopSprint);
}

void AUOW_OptimizationCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	NearestCharacter = FindNearestCharacter();
	UpdateInventoryStats();

	if (IsLocallyControlled())
	{
		EnsureInputMappings();
		UpdateInteractionTarget();
	}
}

AActor* AUOW_OptimizationCharacter::FindNearestCharacter() const
{
	TArray<AActor*> Characters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUOW_OptimizationCharacter::StaticClass(), Characters);

	AActor* Nearest = nullptr;
	float NearestDistance = TNumericLimits<float>::Max();
	for (AActor* Other : Characters)
	{
		if (Other == this)
		{
			continue;
		}

		const float Distance = FVector::Dist(GetActorLocation(), Other->GetActorLocation());
		if (Distance < NearestDistance)
		{
			NearestDistance = Distance;
			Nearest = Other;
		}
	}
	return Nearest;
}

void AUOW_OptimizationCharacter::UpdateInventoryStats()
{
	UInventoryComponent* Inventory = FindComponentByClass<UInventoryComponent>();
	if (!Inventory)
	{
		return;
	}

	InventoryGoldValue = 0;
	for (UItemDataAsset* Item : Inventory->GetItems())
	{
		InventoryGoldValue += Item->GoldCost;
	}

	WeaponCount = Inventory->GetItemsOfType(EItemType::Weapon).Num();
	PotionCount = Inventory->GetItemsOfType(EItemType::Potion).Num();
	IngredientCount = Inventory->GetItemsOfType(EItemType::Ingredient).Num();
	MiscCount = Inventory->GetItemsOfType(EItemType::Misc).Num();
}

void AUOW_OptimizationCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(NewController))
	{
		TeamAgent->SetGenericTeamId(GetGenericTeamId());
	}

	if (NewController && NewController->IsPlayerController())
	{
		ConfigureForPlayer();
	}
}

void AUOW_OptimizationCharacter::ConfigureForPlayer()
{
	GetCharacterMovement()->MaxSimulationIterations = 25;
	GetCharacterMovement()->MaxSimulationTimeStep = 0.001f;

	GetMesh()->SetGenerateOverlapEvents(true);
	GetMesh()->bMultiBodyOverlap = true;

	GetWorldTimerManager().SetTimer(StatsTimer, this, &AUOW_OptimizationCharacter::RefreshPlayerStats, StatsRefreshRate, true);
}

void AUOW_OptimizationCharacter::RefreshPlayerStats()
{
	UpdateInventoryStats();
}

void AUOW_OptimizationCharacter::EnsureInputMappings()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AUOW_OptimizationCharacter::UpdateInteractionTarget()
{
	if (HealthComponent->IsDead())
	{
		SetInteractionTarget(nullptr);
		return;
	}

	const FVector Start = FollowCamera->GetComponentLocation();
	const FVector End = Start + FollowCamera->GetForwardVector() * InteractionDistance;
	const FCollisionQueryParams Params(SCENE_QUERY_STAT(InteractionTrace), true, this);

	const FCollisionShape Shape = FCollisionShape::MakeCapsule(InteractionRadius, InteractionHalfHeight);

	TArray<FHitResult> Hits;
	GetWorld()->SweepMultiByObjectType(Hits, Start, End, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::AllObjects), Shape, Params);

	AActor* NewTarget = nullptr;
	for (const FHitResult& Hit : Hits)
	{
		for (TActorIterator<AItemPickup> It(GetWorld()); It; ++It)
		{
			if (*It == Hit.GetActor() && !NewTarget)
			{
				NewTarget = *It;
			}
		}
	}

	SetInteractionTarget(NewTarget);

#if ENABLE_DRAW_DEBUG
	if (GDebugDrawInteraction)
	{
		DrawInteractionDebug(GetWorld(), Start, End, Shape, Hits, InteractionTarget);
	}
#endif
}

void AUOW_OptimizationCharacter::SetInteractionTarget(AActor* NewTarget)
{
	const bool bHasTarget = NewTarget != nullptr;
	if (NewTarget == InteractionTarget && bHasTarget == bHasInteractionTarget)
	{
		return;
	}

	AActor* PreviousTarget = InteractionTarget;
	const bool bHadTarget = bHasInteractionTarget;
	InteractionTarget = NewTarget;
	bHasInteractionTarget = bHasTarget;

	if (bHadTarget)
	{
		OnInteractionTargetLost.Broadcast(PreviousTarget);
	}

	if (bHasTarget)
	{
		const FText Prompt = NewTarget->Implements<UInteractable>() ? IInteractable::Execute_GetInteractionPrompt(NewTarget) : FText::GetEmpty();
		OnInteractionTargetFound.Broadcast(NewTarget, Prompt);
	}
}

void AUOW_OptimizationCharacter::Interact()
{
	InteractWith(InteractionTarget);
}

void AUOW_OptimizationCharacter::InteractWith(AActor* Target)
{
	if (Target && Target->Implements<UInteractable>() && !HealthComponent->IsDead())
	{
		IInteractable::Execute_Interact(Target, this);
	}
}

void AUOW_OptimizationCharacter::DropLoot()
{
	const TArray<UItemDataAsset*> Items = InventoryComponent->GetItems();
	if (Items.IsEmpty())
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const FVector Location = GetActorLocation() - FVector(0.f, 0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 0.5f);
	if (AItemPickup* Pickup = GetWorld()->SpawnActor<AItemPickup>(AItemPickup::StaticClass(), Location, FRotator::ZeroRotator, SpawnParams))
	{
		Pickup->SetItem(Items[FMath::RandRange(0, Items.Num() - 1)], 1);
	}
}

void AUOW_OptimizationCharacter::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamId = NewTeamID.GetId();

	if (IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(GetController()))
	{
		TeamAgent->SetGenericTeamId(NewTeamID);
	}
}

void AUOW_OptimizationCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AUOW_OptimizationCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AUOW_OptimizationCharacter::Look);

		if (SprintAction)
		{
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AUOW_OptimizationCharacter::StartSprint);
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AUOW_OptimizationCharacter::StopSprint);
		}

		if (LightAttackAction)
		{
			EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Started, CombatComponent, &UCombatComponent::LightAttack);
		}

		if (HeavyAttackAction)
		{
			EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Started, CombatComponent, &UCombatComponent::HeavyAttack);
		}

		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AUOW_OptimizationCharacter::Interact);
		}
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component!"), *GetNameSafe(this));
	}
}

void AUOW_OptimizationCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);

		MovementHistory.Insert(FString::Printf(TEXT("%.2f: %s"), GetWorld()->GetTimeSeconds(), *GetActorLocation().ToString()), 0);
	}
}

void AUOW_OptimizationCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AUOW_OptimizationCharacter::StartSprint()
{
	if (bIsSprinting || HealthComponent->IsDead() || StaminaComponent->IsEmpty())
	{
		return;
	}

	bIsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	StaminaComponent->StartDrain(SprintStaminaDrain);
}

void AUOW_OptimizationCharacter::StopSprint()
{
	if (!bIsSprinting)
	{
		return;
	}

	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	StaminaComponent->StopDrain();
}

void AUOW_OptimizationCharacter::HandleDeath(AController* Killer)
{
	StopSprint();
	DropLoot();
	GetCharacterMovement()->DisableMovement();

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		if (UBrainComponent* Brain = AIController->GetBrainComponent())
		{
			Brain->StopLogic(TEXT("Dead"));
		}
	}
	else if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		DisableInput(PlayerController);
	}
}
