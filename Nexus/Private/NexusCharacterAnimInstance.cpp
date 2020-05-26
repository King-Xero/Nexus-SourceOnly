// Toyan Green © 2020


#include "NexusCharacterAnimInstance.h"
#include "NexusCharacter.h"
#include "GameFramework/PawnMovementComponent.h"

UNexusCharacterAnimInstance::UNexusCharacterAnimInstance()
	: JumpEnabled(false)
	, IsInAir(false)
	, IsCrouching(false)
	, Speed(0.0f)
	, Direction(0.0f)
{
}

void UNexusCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// Cache owner for later use
	Owner = TryGetPawnOwner();
}

void UNexusCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Owner && Owner->IsA(ANexusCharacter::StaticClass()))
	{
		const auto cPlayerCharacter = Cast<ANexusCharacter>(Owner);

		if (cPlayerCharacter)
		{
			// Set animation variables
			JumpEnabled = cPlayerCharacter->bWasJumping;
			IsInAir = cPlayerCharacter->GetMovementComponent()->IsFalling();
			IsCrouching = cPlayerCharacter->bIsCrouched;
			Speed = cPlayerCharacter->GetVelocity().Size();
			Direction = CalculateDirection(cPlayerCharacter->GetVelocity(), cPlayerCharacter->GetActorRotation());
			IsDead = cPlayerCharacter->GetIsCharacterDead();
		}
	}
}