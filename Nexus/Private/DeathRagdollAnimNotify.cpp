// Toyan Green © 2020


#include "DeathRagdollAnimNotify.h"
#include "NexusCharacter.h"

void UDeathRagdollAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (nullptr != MeshComp && nullptr != MeshComp->GetOwner())
	{
		ANexusCharacter* NexusCharacter = Cast<ANexusCharacter>(MeshComp->GetOwner());

		if (nullptr != NexusCharacter)
		{
			// Trigger ragdoll for character and weapon.
			NexusCharacter->DeathRagdollCharacter();
		}
	}
}