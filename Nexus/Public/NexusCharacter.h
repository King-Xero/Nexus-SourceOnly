// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NexusWeapon.h"
#include "NexusCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UNexusHealthComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnADSUpdatedSignature, ANexusCharacter*, Character, bool, bAmingDownSights);

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

	/**
	 * \brief Check if the character is dead.
	 * \return true - dead, false - alive.
	 */
	bool IsDead() const;

	/**
	 * \brief Check if the character is aiming down sights.
	 * \return true - ADS, false - Not ADS.
	 */
	bool IsAimingDownSights() const;

	/**
	 * \brief Make the character start firing the currently equipped weapon.
	 */
	UFUNCTION(BlueprintCallable, Category = "Player")
	void StartShooting();

	/**
	 * \brief Make the character stop firing the currently equipped weapon.
	 */
	UFUNCTION(BlueprintCallable, Category = "Player")
	void StopShooting();

	/**
	 * \brief Make the character start reloading the currently equipped weapon.
	 */
	UFUNCTION(BlueprintCallable, Category = "Player")
	void StartReloading();

	/**
	 * \brief Make the character and currently equipped weapon ragdoll.
	 */
	UFUNCTION(BlueprintCallable, Category = "Player")
	void DeathRagdollCharacter() const;

	/**
	 * \brief Event used to broadcast ammo updates.
	 */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnADSUpdatedSignature OnADSUpdated;

	/**
	 * \brief Grass footstep sound effect.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
	USoundBase* GrassFootstepSFX;

	/**
	 * \brief Crouch footstep sound effect.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
	USoundBase* CrouchFootstepSFX;

	/**
	 * \brief Footstep sound effect.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
	USoundBase* FootstepSFX;

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

	/**
	 * \brief Make the character crouch from standing.
	 */
	UFUNCTION(BlueprintCallable, Category = "Player")
	void StartCrouch();

	/**
	 * \brief Make the character stand from crouching.
	 */
	UFUNCTION(BlueprintCallable, Category = "Player")
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
	 * \brief Respond to a change in health. Wired up to health components OnHealthChanged event. Uses the signature for FOnHealthChangedSignature.
	 * \param HealthComponent The health component the experienced a health change.
	 * \param Health The amount of health the component now has.
	 * \param HealthDelta The amount of health that changed.
	 * \param DamageType The type of damage that was being inflicted.
	 * \param InstigatedBy The controller that inflicted the health change. (specific player or ai)
	 * \param DamageCauser The actor that inflicted the health change. (player, weapon, etc)
	 */
	UFUNCTION()
	virtual void HealthChanged(UNexusHealthComponent* HealthComponent, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

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
	 * \brief The weapon the character is currently using.
	 */
	UPROPERTY(Replicated)
	ANexusWeapon* CurrentWeapon;

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
	 * \brief The number of seconds until the character is destroyed, after the character dies. (0 = character will not be destroyed)
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
	float DeathLifeSpan = 10.0f;

	/**
	 * \brief Sound effect spawned at the character's location when they crouch.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
	USoundBase* CrouchSFX;

	/**
	 * \brief Sound effect spawned at the character's location when they die.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
	USoundBase* DeathSFX;

	/**
	 * \brief Animations played when the character dies.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
	TArray<UAnimSequence*> DeathAnimations;
	
private:

	/**
	 * \brief Set the camera field of view to set aim zoom.
	 * \param DeltaTime Time since last update.
	 */
	void SetAimDownSight(float DeltaTime);

	/**
	 * \brief Spawn sound effect for character crouching.
	 */
	void PlayCrouchSFX() const;

	/**
	 * \brief Spawn sound effect for character dying.
	 */
	void PlayDeathSFX() const;

	/**
	 * \brief Play random death animation from available animations.
	 */
	void PlayDeathAnimation() const;
	
	/**
	 * \brief Default FOV value for the camera, cached on begin play.
	 */
	float DefaultFOV;

	/**
	 * \brief Used to toggle the ADS zoom.
	 */
	UPROPERTY(Replicated)
	bool bAimDownSight;

	/**
	 * \brief Used to track if the character is dead.
	 */
	UPROPERTY(Replicated)
	bool bDead;

	/**
	 * \brief Default maximum walk speed, cached on begin play.
	 */
	float DefaultMaxWalkSpeed;
		
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

	/**
	 * \brief Name used for reload input binding.
	 */
	const FName ReloadBindingName = "Reload";

	/**
	 * \brief Weighting used to blend between ragdoll physics and animation.
	 */
	const float RagdollPhysicsBlendWeight = 0.8f;
};
