// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexusWeapon.generated.h"

class ANexusCharacter;
class USkeletalMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWeaponAmmoUpdatedSignature, ANexusWeapon*, Weapon, int32, NewAmmoInClip, int32, NewAmmoInReserve);

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

/**
 * \brief Used to track the current weapon activity.
 */
UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	/**
	 * \brief Invalid weapon state.
	 */
	Unknown				UMETA(DisplayName = "Unknown"),
	/**
	 * \brief Weapon is equipped, but not doing anything.
	 */
	Idle				UMETA(DisplayName = "Idle"),
	/**
	 * \brief Weapon is equipped and being fired.
	 */
	Firing				UMETA(DisplayName = "Firing"),
	/**
	 * \brief Weapon is equipped and being reloaded.
	 */
	Reloading			UMETA(DisplayName = "Reloading"),
	/**
	 * \brief Weapon is equipped and being swapped out.
	 */
	Swapping			UMETA(DisplayName = "Swapping"),
	/**
	 * \brief Weapon is not equipped.
	 */
	Offhand				UMETA(DisplayName = "Offhand"),	
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
	virtual void StartFiring() {}

	/**
	 * \brief Stop shooting the weapon.
	 */
	virtual void StopFiring();

	/**
	 * \brief Start reloading the weapon.
	 */
	virtual void StartReloading();

	/**
	 * \brief Stop reloading the weapon.
	 */
	virtual void StopReloading();

	/**
	 * \brief Set the actor owner.
	 * \param NewOwningCharacter
	 */
	void SetOwningCharacter(ANexusCharacter* NewOwningCharacter);

	/**
	 * \brief Return the amount of ammo currently in the clip.
	 * \return Amount of ammo.
	 */
	int32 GetAmmoInClip() const;

	/**
	 * \brief Return the amount of ammo currently in reserve.
	 * \return Amount of ammo.
	 */
	int32 GetAmmoInReserve() const;
	
	/**
	 * \brief Return the maximum ammo this weapon can carry.
	 * \return Max ammo capacity.
	 */
	int32 GetMaxAmmoCapacity() const;

	/**
	 * \brief Return the crosshair texture to be used when firing from the hip.
	 * \return Crosshair texture.
	 */
	UTexture* GetHipFireCrosshairTexture() const;

	/**
	 * \brief Return the crosshair texture to be used when firing whilst aiming down sights.
	 * \return Crosshair texture.
	 */
	UTexture* GetADSCrosshairTexture() const;

	/**
	 * \brief Return the name of the weapon.
	 * \return Weapon name.
	 */
	FName GetWeaponName() const;

	/**
	 * \brief Get the current weapon state.
	 * \return CurrentWeaponState
	 */
	EWeaponState GetWeaponState() const;

	/**
	 * \brief Get the transform of the IK socket for this weapon.
	 * \return IK Socket transform.
	 */
	FTransform GetIKSocketTransform() const;

	/**
	 * \brief Check if the weapon can be swapped out.
	 * \return 
	 */
	bool CanWeaponBeSwapped() const;
	
	/**
	 * \brief Set the current state of the weapon.
	 * \param NewWeaponState
	 */
	void SetWeaponState(EWeaponState NewWeaponState);

	/**
	 * \brief Restore weapon ammo.
	 */
	virtual void RestoreAmmo(int32 AmmoAmount);
	
	/**
	 * \brief Event used to broadcast ammo updates.
	 */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnWeaponAmmoUpdatedSignature OnAmmoUpdated;
	
protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/**
	 * \brief Check if the weapon can be fired.
	 * \return Can fire - true, Cannot fire - false.
	 */
	bool CanFireWeapon() const;

	/**
	 * \brief Check if the weapon has ammo in the clip.
	 * \return Has ammo - true, No ammo - false.
	 */
	bool HasAmmoInClip() const;

	/**
	 * \brief Fire a line trace to apply damage and play effects on anything hit.
	 * \param WeaponOwner The owner that fired the weapon. Used to apply damage.
	 * \param BulletTracerTargetOut Location of the hit, or the end of the line trace.
	 * \param SurfaceTypeOut The type of surface that was hit. Used to determine damage multipliers and effects to be played.
	 */
	void LineTraceForDamageAndImpactEffects(AActor* WeaponOwner, FVector& BulletTracerTargetOut, EPhysicalSurface& SurfaceTypeOut);
	
	/**
	 * \brief Shoot the weapon.
	 */
	virtual void Fire() { }

	/**
	 * \brief Shoot the weapon on the server.
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	/**
	 * \brief Replicate weapon hit effects.
	 */
	UFUNCTION()
	void OnRep_HitScanInfo();

	/**
	 * \brief Check if the weapon can be reloaded.
	 * \return Can reload - true, Cannot reload - false.
	 */
	bool CanReloadWeapon();
	
	/**
	 * \brief Reload the weapon.
	 */
	virtual void Reload();

	/**
	 * \brief Reload the weapon on the server.
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerReload();

	/**
	 * \brief Restore weapon ammo on the server.
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRestoreAmmo(int32 AmmoAmount);
	
	/**
	 * \brief Call the server to play an animation montage.
	 */
	UFUNCTION(Server, Reliable)
	void ServerPlayAnimationMontage(UAnimMontage* AnimMontage, float PlaybackRate = 1.0f);
	
	/**
	 * \brief Execute the animation montage on the server and all clients.
	 */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayAnimationMontage(UAnimMontage* AnimMontage, float PlaybackRate);

	/**
	 * \brief Call the server to play a sound effect attached to the weapon.
	 */
	UFUNCTION(Server, Reliable)
	void ServerPlaySFX(USoundBase* SoundEffect);

	/**
	 * \brief Play the attached sound effect on the server and all clients.
	 */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlaySFX(USoundBase* SoundEffect);

	/**
	 * \brief Call the server to play a sound effect at the given location.
	 */
	UFUNCTION(Server, Reliable)
	void ServerPlaySFXAtLocation(USoundBase* SoundEffect, FVector Location);

	/**
	 * \brief Play the sound effect on the server and all clients.
	 */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlaySFXAtLocation(USoundBase* SoundEffect, FVector Location);
	
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
	 * \brief The bullet spread angle of the weapon (in degrees) while aiming down sights.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (ClampMin = 0.0f))
	float ADSBulletSpreadAngle = 0.5f;

	/**
	 * \brief The amount of ammo this weapon spawns with.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	int32 StartingAmmo = 999;

	/**
	 * \brief The total amount of currently available ammo.
	 */
	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = "Weapon")
	int32 CurrentTotalAmmo;

	/**
	 * \brief The maximum amount of ammo that can be carried for this weapon.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	int32 MaxAmmo = 999;

	/**
	 * \brief The amount of ammo currently loaded in the clip.
	 */
	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = "Weapon")
	int32 CurrentAmmoInClip;
	
	/**
	 * \brief The maximum amount of ammo that can be loaded into the clip.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	int32 MaxAmmoPerClip = 30;

	/**
	 * \brief The time it takes to reload the weapon.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float WeaponReloadDelayTime = 2.0f;
		
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
	 * \brief Reload animation montage for hip fire.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UAnimMontage* HipReloadAnimMontage;

	/**
	 * \brief Reload animation montage for ADS.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UAnimMontage* ADSReloadAnimMontage;

	/**
	 * \brief Firing animation montage for hip fire.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UAnimMontage* HipFireAnimMontage;

	/**
	 * \brief Firing animation montage for ADS.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UAnimMontage* ADSFireAnimMontage;

	/**
	 * \brief Sound effect spawned at the weapons's location when it is fired.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	USoundBase* FiredSFX;

	/**
	 * \brief Sound effect spawned at the weapons's location when it is fired with no ammo in the clip.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	USoundBase* DryFiredSFX;

	/**
	 * \brief Sound effect spawned at the hit location when the weapon shot hits a characters head.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	USoundBase* HeadshotImpactSFX;

	/**
	 * \brief Sound effect spawned at the hit location when the weapon shot hits a characters body.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	USoundBase* BodyImpactSFX;

	/**
	 * \brief Sound effect spawned at the hit location when the weapon shot hits a characters limb.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	USoundBase* LimbImpactSFX;

	/**
	 * \brief Crosshair texture used when firing from the hip.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UTexture* HipFireCrosshairTexture;

	/**
	 * \brief Crosshair texture used when firing while aiming down sights.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UTexture* ADSCrosshairTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName WeaponName;

	/**
	 * \brief Hit scan information for replication.
	 */
	UPROPERTY(ReplicatedUsing=OnRep_HitScanInfo)
	FHitScanInfo HitScanInfo;

	/**
	 * \brief The character that currently owns this weapon.
	 */
	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = "Weapon")
	ANexusCharacter* OwningCharacter;

	/**
	 * \brief Used to track if the weapon is currently reloading/swapping/firing.
	 */
	UPROPERTY(Replicated)
	EWeaponState CurrentWeaponState;

	/**
	 * \brief Play all effects for when the weapon hits something.
	 * \param SurfaceType The type of surface the weapon hit.
	 * \param Target The location of the impact.
	 */
	void PlayWeaponImpactEffects(EPhysicalSurface SurfaceType, FVector Target);

	/**
	 * \brief Spawn particle effect for weapon impact.
	 * \param SurfaceType The type of surface that was hit.
	 * \param Target The location of the impact
	 */
	void PlayImpactEffect(EPhysicalSurface SurfaceType, FVector Target);
	
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
	 * \brief Spawn sound effect for weapon fired.
	 */
	void PlayFiredSFX();

	/**
	 * \brief Spawn sound effect for weapon fired with no ammo.
	 */
	void PlayDryFiredSFX();

	/**
	 * \brief Play the fired animation on the owning player.
	 */
	void PlayFiredAnimation();

	/**
	 * \brief Get the damage multiplier for the surface type that was hit.
	 * \param SurfaceType The type of surface that was hit.
	 * \return Multiplier used to scale damage.
	 */
	float GetDamageMultiplier(EPhysicalSurface SurfaceType) const;

	/**
	 * \brief Remove ammo from the clip when fired.
	 */
	void DepleteAmmo();

	/**
	 * \brief Handle used to manage the weapon fire delay timer.
	 */
	FTimerHandle TimerHandle_WeaponFireDelay;

	/**
	 * \brief Handle used to manage the weapon reload delay timer.
	 */
	FTimerHandle TimerHandle_WeaponReloadDelay;

	/**
	 * \brief The last time the weapon fired. (Cached when the weapon fire method executes)
	 */
	float LastFireTime;

	/**
	 * \brief The time delay between weapon shots. (Calculated using WeaponRateOfFire)
	 */
	float WeaponFireDelayTime;

	/**
	 * \brief Name of the weapon IK socket.
	 */
	const FName LeftHandIKSocketName = "LeftHandIKSocket";
};
