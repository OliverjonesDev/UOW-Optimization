#include "Animation/AttackHitNotifyState.h"
#include "Components/CombatComponent.h"
#include "Components/SkeletalMeshComponent.h"

void UAttackHitNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (UCombatComponent* Combat = MeshComp ? MeshComp->GetOwner()->FindComponentByClass<UCombatComponent>() : nullptr)
	{
		Combat->BeginHitDetection();
	}
}

void UAttackHitNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (UCombatComponent* Combat = MeshComp ? MeshComp->GetOwner()->FindComponentByClass<UCombatComponent>() : nullptr)
	{
		Combat->PerformHitTrace();
	}
}

void UAttackHitNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (UCombatComponent* Combat = MeshComp ? MeshComp->GetOwner()->FindComponentByClass<UCombatComponent>() : nullptr)
	{
		Combat->EndHitDetection();
	}
}
