// Toyan Green © 2020

#include "FootstepSoundAnimNotify.h"
#include "NexusCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Nexus/Utils/NexusTypeDefinitions.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"

void UFootstepSoundAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (nullptr != MeshComp && nullptr != MeshComp->GetOwner())
	{
		ANexusCharacter* NexusCharacter = Cast<ANexusCharacter>(MeshComp->GetOwner());

		if (nullptr != NexusCharacter)
		{
			UCharacterMovementComponent* CharacterMovement = NexusCharacter->GetCharacterMovement();

			// The character should always have a movement component
			checkf(CharacterMovement != nullptr, TEXT("Character movement component missing from character"));

			// Check if the character is touching the floor. Footsteps should not be played when in the air.
			if (CharacterMovement->CurrentFloor.bBlockingHit)
			{
				// Check if the current "floor" is grass.
				if (SURFACE_ENVIRONMENT_GRASS == UPhysicalMaterial::DetermineSurfaceType(CharacterMovement->CurrentFloor.HitResult.PhysMaterial.Get()))
				{
					if (NexusCharacter->GrassFootstepSFX)
					{
						UGameplayStatics::SpawnSoundAttached(NexusCharacter->GrassFootstepSFX, MeshComp);
					}
				}
				else
				{
					if (CharacterMovement->IsCrouching())
					{
						if (NexusCharacter->CrouchFootstepSFX)
						{
							UGameplayStatics::SpawnSoundAttached(NexusCharacter->CrouchFootstepSFX, MeshComp);
						}
					}
					else
					{
						if (NexusCharacter->FootstepSFX)
						{
							UGameplayStatics::SpawnSoundAttached(NexusCharacter->FootstepSFX, MeshComp);
						}
					}
				}
			}
		}
	}
}
