// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexusWeapon.generated.h"

class USkeletalMeshComponent;

UCLASS()
class NEXUS_API ANexusWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANexusWeapon();

protected:

	/**
	 * \brief The visible mesh of the weapon.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* MeshComponent;

	/**
	 * \brief Range within which the weapon can hit a target.
	 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float WeaponRange = 10000;

	/**
	 * \brief The amount of damage this weapon deals.
	 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float WeaponDamage = 20.0f;
	
	/**
	 * \brief The type of damage that the weapon inflicts.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	/**
	 * \brief Name of the socket at the weapon's muzzle.
	 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName = "MuzzleFlashSocket";

	/**
	 * \brief Particle effect spawned at the weapon's muzzle when the weapon is fired.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* MuzzleVFX;

	/**
	 * \brief Particle effect spawned at the weapon impact location when a target is hit.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* ImpactVFX;

	/**
	 * \brief Particle effect spawned between the muzzle and shot target location the weapon is fired.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* TracerVFX;

	/**
	 * \brief Name of the bullet tracer effect's target vector parameter.
	 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName TracerTargetParameterName = "BeamEnd";

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/**
	 * \brief Shoot the weapon
	 */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Fire();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
};
