// Toyan Green © 2020


#include "NexusCharacterAnimInstance.h"
#include "NexusCharacter.h"
#include "GameFramework/PawnMovementComponent.h"

UNexusCharacterAnimInstance::UNexusCharacterAnimInstance()
	: bJumpEnabled(false)
	, bInAir(false)
	, bCrouching(false)
	, Speed(0.0f)
	, Direction(0.0f)
	, bLeftHandHoldingWeapon(false)
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
			bJumpEnabled = cPlayerCharacter->bWasJumping;
			bInAir = cPlayerCharacter->GetMovementComponent()->IsFalling();
			bCrouching = cPlayerCharacter->bIsCrouched;
			Speed = cPlayerCharacter->GetVelocity().Size();
			Direction = CalculateDirection(cPlayerCharacter->GetVelocity(), cPlayerCharacter->GetActorRotation());
			bDead = cPlayerCharacter->IsDead();
			bAimingDownSights = cPlayerCharacter->IsAimingDownSights();
			AimPitch = cPlayerCharacter->GetAimPitch();
			AimYaw = cPlayerCharacter->GetAimYaw();
			bLeftHandHoldingWeapon = cPlayerCharacter->IsLeftHandHoldingWeapon();
			LeftHandWeaponIKSocketTransform = cPlayerCharacter->GetLeftHandWeaponIKSocketTransform();
		}
	}
}