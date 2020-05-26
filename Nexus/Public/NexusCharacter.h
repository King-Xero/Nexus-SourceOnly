// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NexusWeapon.h"
#include "NexusCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UNexusHealthComponent;

UCLASS()
class NEXUS_API ANexusCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANexusCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/**
	 * \brief Return characters eye location.
	 * \return Eye location
	 */
	virtual FVector GetPawnViewLocation() const override;

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

	/**
	 * \brief Component used to manage health.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNexusHealthComponent* CharacterHealthComponent;

	/**
	 * \brief Field of view value for aiming down sights.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
	float AimingFOV = 65.0f;

	/**
	 * \brief Speed of the aim down sight interpolation.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
	float ADSInterpolationSpeed = 20.0f;

	/**
	 * \brief Weapon that the character should spawn with.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
	TSubclassOf<ANexusWeapon> SpawnWeaponClass;

	/**
	 * \brief Name of the socket at the weapon's muzzle.
	 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Player")
	FName WeaponSocketName = "hand_rSocket";

	/**
	 * \brief The number of seconds until the character is destroyed, after the character dies. (0 = character will not be destroyed)
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
	float DeathLifeSpan = 10.0f;
	
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
	 * \brief Make the character crouch from standing.
	 */
	void StartCrouch();

	/**
	 * \brief Make the character stand from crouching.
	 */
	void EndCrouch();

	/**
	 * \brief Make the character start "aiming down sight".
	 */
	void StartADS();

	/**
	 * \brief Make the character stop "aiming down sight".
	 */
	void EndADS();

	/**
	 * \brief Make the character start firing the currently equipped weapon.
	 */
	void StartShooting();

	/**
	 * \brief Make the character stop firing the currently equipped weapon.
	 */
	void StopShooting();

	/**
	 * \brief Respond to a change in health. Wired up to health components OnHealthChanged event. Uses the signature for FOnHealthChangedSignature.
	 * \param HealthComponent The health component the experienced a health change.
	 * \param Health The amount of health the component now has.
	 * \param HealthDelta The amount of health that changed.
	 * \param DamageType The type of damage that was being inflicted.
	 * \param InstigatedBy The controller that inflicted the health change. (specific player or ai)
	 * \param DamageCauser The actor that inflicted the health change. (player, weapon, etc)
	 */
	UFUNCTION()
	void HealthChanged(UNexusHealthComponent* HealthComponent, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
	
private:

	/**
	 * \brief Set the camera field of view to set aim zoom.
	 * \param DeltaTime Time since last update.
	 */
	void SetAimDownSight(float DeltaTime);
	
	/**
	 * \brief Default FOV value for the camera, cached on begin play.
	 */
	float DefaultFOV;

	/**
	 * \brief Used to toggle the ADS zoom.
	 */
	bool bShouldAimDownSight;

	/**
	 * \brief The weapon the character is currently using.
	 */
	ANexusWeapon* CurrentWeapon;

	/**
	 * \brief Used to track if the character is dead.
	 */
	bool bIsCharacterDead;
		
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

	/**
	 * \brief Name used for jump input binding.
	 */
	const FName JumpBindingName = "Jump";

	/**
	 * \brief Name used for aiming input binding.
	 */
	const FName AimingBindingName = "AimDownSight";

	/**
	 * \brief Name used for shooting input binding.
	 */
	const FName ShootBindingName = "Fire";
};
