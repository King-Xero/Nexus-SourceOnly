// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NexusCharacter.generated.h"

UCLASS()
class NEXUS_API ANexusCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANexusCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/**
	 * \brief Moves the character forward or backward, depending on axis value.
	 * \param fAxisValue Value of the input axis.
	 */
	void MoveForward(float fAxisValue);

	/**
	 * \brief Moves the character left or right, depending on axis value.
	 * \param fAxisValue Value of the input axis.
	 */
	void MoveRight(float fAxisValue);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	
	/**
	 * \brief Name used for move forward input binding.
	 */
	FName kstrMoveForwardBinding = "MoveForward";

	/**
	 * \brief Name used for move right input binding.
	 */
	FName kstrMoveRightBinding = "MoveRight";

	/**
	 * \brief Name used for look up input binding.
	 */
	FName kstrLookUpBinding = "LookUp";

	/**
	 * \brief Name used for turn input binding.
	 */
	FName kstrTurnBinding = "Turn";
};
