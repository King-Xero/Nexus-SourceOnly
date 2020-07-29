// Toyan Green © 2020


#include "WeaponSwapAnimNotifyState.h"
#include "NexusCharacter.h"

void UWeaponSwapAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (nullptr != MeshComp && nullptr != MeshComp->GetOwner())
	{
		ANexusCharacter* NexusCharacter = Cast<ANexusCharacter>(MeshComp->GetOwner());

		if (nullptr != NexusCharacter)
		{
			NexusCharacter->SwapWeapon();
		}
	}
}

void UWeaponSwapAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (nullptr != MeshComp && nullptr != MeshComp->GetOwner())
	{
		ANexusCharacter* NexusCharacter = Cast<ANexusCharacter>(MeshComp->GetOwner());

		if (nullptr != NexusCharacter)
		{
			// Get the equipped weapon ready to use.
			NexusCharacter->PrepareWeaponAfterSwap();
		}
	}
}
