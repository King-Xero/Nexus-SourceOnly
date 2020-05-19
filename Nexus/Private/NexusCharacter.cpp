// Toyan Green © 2020


#include "NexusCharacter.h"

// Sets default values
ANexusCharacter::ANexusCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ANexusCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

/**
 * \brief Moves the character forward or backward, depending on axis value.
 * \param fAxisValue Value of the input axis.
 */
void ANexusCharacter::MoveForward(float fAxisValue)
{
	AddMovementInput(GetActorForwardVector(), fAxisValue);
}

/**
 * \brief Moves the character left or right, depending on axis value.
 * \param fAxisValue Value of the input axis.
 */
void ANexusCharacter::MoveRight(float fAxisValue)
{
	AddMovementInput(GetActorRightVector(), fAxisValue);
}

// Called every frame
void ANexusCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ANexusCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(kstrMoveForwardBinding, this, &ANexusCharacter::MoveForward);
	PlayerInputComponent->BindAxis(kstrMoveRightBinding, this, &ANexusCharacter::MoveRight);
}