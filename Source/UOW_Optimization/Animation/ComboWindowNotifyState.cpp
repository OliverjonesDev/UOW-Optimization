#include "Animation/ComboWindowNotifyState.h"
#include "Components/CombatComponent.h"
#include "Components/SkeletalMeshComponent.h"

void UComboWindowNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (UCombatComponent* Combat = MeshComp ? MeshComp->GetOwner()->FindComponentByClass<UCombatComponent>() : nullptr)
	{
		Combat->OpenComboWindow();
	}
}

void UComboWindowNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (UCombatComponent* Combat = MeshComp ? MeshComp->GetOwner()->FindComponentByClass<UCombatComponent>() : nullptr)
	{
		Combat->CloseComboWindow();
	}
}
