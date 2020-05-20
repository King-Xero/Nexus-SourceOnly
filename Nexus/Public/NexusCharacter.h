// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NexusCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class NEXUS_API ANexusCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANexusCharacter();

protected:

	/**
	 * \brief Third person camera component
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComponent;

	/**
	 * \brief Component used to mount the camera.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComponent;
	
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

	/**
	 * \brief Make the character crouch from standing
	 */
	void StartCrouch();

	/**
	 * \brief Make the character stand from crouching
	 */
	void EndCrouch();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	
	/**
	 * \brief Name used for move forward input binding.
	 */
	const FName MoveForwardBindingName = "MoveForward";

	/**
	 * \brief Name used for move right input binding.
	 */
	const FName MoveRightBindingName = "MoveRight";

	/**
	 * \brief Name used for look up input binding.
	 */
	const FName LookUpBindingName = "LookUp";

	/**
	 * \brief Name used for turn input binding.
	 */
	const FName TurnBindingName = "Turn";

	/**
	 * \brief Name used for crouch input binding.
	 */
	const FName CrouchBindingName = "Crouch";
};
