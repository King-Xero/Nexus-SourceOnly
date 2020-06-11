// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexusWeapon.generated.h"

class USkeletalMeshComponent;

USTRUCT()
struct FHitScanInfo
{
	GENERATED_USTRUCT_BODY()

	/**
	 * \brief The type of surface that was hit.
	 */
	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> HitSurfaceType;

	/**
	 * \brief End location for hit scan trace.
	 */
	UPROPERTY()
	FVector_NetQuantize TraceTargetLocation;
};

UCLASS()
class NEXUS_API ANexusWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANexusWeapon();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/**
	 * \brief Start shooting the weapon.
	 */
	virtual void StartFiring();

	/**
	 * \brief Stop shooting the weapon.
	 */
	virtual void StopFiring();	
	
protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	/**
	 * \brief Shoot the weapon.
	 */
	virtual void Fire();

	/**
	 * \brief Shoot the weapon on the server.
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	/**
	 * \brief Replicate weapon hit effects.
	 */
	UFUNCTION()
	void OnRep_HitScanInfo() const;
	
	/**
	 * \brief The visible mesh of the weapon.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* MeshComponent;

	/**
	 * \brief Range within which the weapon can hit a target.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float WeaponRange = 10000;

	/**
	 * \brief The amount of damage this weapon deals.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float WeaponDamage = 20.0f;

	/**
	 * \brief The damage multiplier applied when this weapon hits a character's head.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float HeadShotDamageMultiplier = 5.0f;

	/**
	 * \brief The damage multiplier applied when this weapon hits a character's limbs.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float LimbDamageMultiplier = 0.5f;

	/**
	 * \brief The firing rate of the weapon. (Rounds Per Minute)
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float WeaponRateOfFire = 60.0f;

	/**
	 * \brief The bullet spread angle of the weapon (in degrees).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (ClampMin=0.0f))
	float BulletSpreadAngle = 2.0f;
		
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
	 * \brief Particle effect spawned at the weapon impact location when something is hit.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* DefaultImpactVFX;
	
	/**
	 * \brief Particle effect spawned at the weapon impact location when a character is hit.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* CharacterImpactVFX;

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

	/**
	 * \brief Camera shake triggered when the weapon is fired.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UCameraShake> WeaponCameraShake;

	/**
	 * \brief Hit scan information for replication.
	 */
	UPROPERTY(ReplicatedUsing=OnRep_HitScanInfo)
	FHitScanInfo HitScanInfo;

	/**
	 * \brief Play all effects for when the weapon hits something.
	 * \param SurfaceType The type of surface the weapon hit.
	 * \param Target The location of the impact.
	 */
	void PlayWeaponImpactEffects(EPhysicalSurface SurfaceType, FVector Target) const;

	/**
	 * \brief Spawn particle effect for weapon impact.
	 * \param SurfaceType The type of surface that was hit.
	 * \param Target The location of the impact
	 */
	void PlayImpactEffect(EPhysicalSurface SurfaceType, FVector Target) const;
	
	/**
	 * \brief Play all effects for when the weapon is fired.
	 * \param Target Weapon hit location.
	 */
	void PlayWeaponFiredEffects(FVector Target) const;
	
	/**
	 * \brief Spawn particle effect for muzzle flash.
	 */
	void PlayMuzzleEffect() const;

	/**
	 * \brief Spawn bullet tracer particle effect.
	 * \param Target Weapon hit location.
	 */
	void PlayBulletTracerEffect(FVector Target) const;

	/**
	 * \brief Shake the owning characters camera when the weapon is fired.
	 */
	void PlayCameraShake() const;

	/**
	 * \brief Get the damage multiplier for the surface type that was hit.
	 * \param SurfaceType The type of surface that was hit.
	 * \return Multiplier used to scale damage.
	 */
	float GetDamageMultiplier(EPhysicalSurface SurfaceType) const;

	/**
	 * \brief Handle used to manage the weapon fire delay timer.
	 */
	FTimerHandle TimerHandle_WeaponFireDelay;

	/**
	 * \brief The last time the weapon fired. (Cached when the weapon fire method executes)
	 */
	float LastFireTime;

	/**
	 * \brief The time delay between weapon shots. (Calculated using WeaponRateOfFire)
	 */
	float WeaponFireDelayTime;
};
