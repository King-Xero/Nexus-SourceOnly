// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ExplodingEnemy.generated.h"

class UNexusHealthComponent;
class URadialForceComponent;
class USphereComponent;
class USoundCue;

UCLASS()
class NEXUS_API AExplodingEnemy : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AExplodingEnemy();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/**
	 * \brief Event when this actor overlaps another actor, for example a player walking into a trigger.
	 * \param OtherActor The actor that was overlapped.
	 *	@note Components on both this and the other Actor must have bGenerateOverlapEvents set to true to generate overlap events.
	 */
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/**
	 * \brief Get the next location the enemy should move to.
	 * \return The point location.
	 */
	FVector GetNextPathPoint();

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

	/**
	 * \brief Replicate enemy damaged effects.
	 */
	UFUNCTION()
	void OnRep_Damaged() const;
	
	/**
	 * \brief Explode the enemy.
	 */
	UFUNCTION(BlueprintCallable, Category = "ExplodingEnemy")
	void Explode();

	/**
	 * \brief Replicate enemy explosion effects.
	 */
	UFUNCTION()
	void OnRep_Explode() const;
	
	/**
	 * \brief The visible mesh of the enemy.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	/**
	 * \brief Component used to manage health.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNexusHealthComponent* EnemyHealthComponent;
	
	/**
	 * \brief Component used to emit radial force when the enemy explodes.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	URadialForceComponent* RadialForceComponent;

	/**
	 * \brief Collision sphere used to detect nearby actors.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereComponent;

	/**
	 * \brief Component used to play movement sound effect.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAudioComponent* MovementAudioComponent;

	/**
	 * \brief Particle effect spawned at the enemy's location when it explodes.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ExplodingEnemy")
	UParticleSystem* ExplosionVFX;

	/**
	 * \brief Sound effect spawned attached to the enemy when it starts to self destruct.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ExplodingEnemy")
	USoundBase* SelfDestructSFX;

	/**
	 * \brief Sound effect spawned at the enemy's location when it explodes.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ExplodingEnemy")
	USoundBase* ExplosionSFX;

	/**
	 * \brief Sound effect for enemy movement.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ExplodingEnemy")
	USoundBase* MovementSFX;

	/**
	 * \brief The magnitude of the movement force applied to the enemy when moving in the direction of its next path point.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "ExplodingEnemy")
	float MovementForce;

	/**
	 * \brief How close the enemy needs to be to a movement target for positions to be considered equal.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "ExplodingEnemy")
	float RequiredDistanceToTarget;

	/**
	 * \brief Flag to set whether the movement force applied should be a change in velocity.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "ExplodingEnemy")
	bool bVelocityChange;

	/**
	 * \brief Magnitude of the radial impulse emitted from the enemy when it explodes.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ExplodingEnemy")
	float RadialImpulseStrength = 500.0f;

	/**
	 * \brief The amount of damage dealt when the enemy explodes.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ExplodingEnemy")
	float ExplosionDamage = 20.0f;

	/**
	 * \brief The distance within which the explosion deals damage.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ExplodingEnemy")
	float ExplosionRadius = 200.0f;

	/**
	 * \brief The time it take for this enemy to self destruct after overlapping a player.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ExplodingEnemy")
	float SelfDestructTime = 3.0f;

	/**
	 * \brief The lower bound of the enemy velocity to be mapped to, mapped LowerVolumeRollingRange.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ExplodingEnemy")
	float LowerVelocityRollingRange = 10.0f;
	
	/**
	 * \brief The upper bound of the enemy velocity to be mapped to, mapped UpperVolumeRollingRange.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ExplodingEnemy")
	float UpperVelocityRollingRange = 1000.0f;

	/**
	 * \brief The lower bound of the enemy movement volume to be mapped to LowerVelocityRollingRange.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ExplodingEnemy")
	float LowerVolumeRollingRange = 0.1f;

	/**
	 * \brief The upper bound of the enemy movement volume to be mapped to UpperVelocityRollingRange.
	 */
	float UpperVolumeRollingRange = 2.0f;

	/**
	 * \brief The type of damage that the enemy inflicts.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ExplodingEnemy")
	TSubclassOf<UDamageType> EnemyDamageType;

private:
	/**
	 * \brief Spawn particle effect for explosion.
	 */
	void PlayExplosionVFX() const;

	/**
	 * \brief Spawn sound effect for self destruct.
	 */
	void PlaySelfDestructSFX() const;

	/**
	 * \brief Spawn sound effect for explosion.
	 */
	void PlayExplosionSFX() const;

	/**
	 * \brief Used to track if the enemy has exploded.
	 */
	UPROPERTY(ReplicatedUsing = OnRep_Explode)
	bool bExploded;

	
	/**
	 * \brief Used to pulse the enemy's material when it taken damage.
	 */
	UPROPERTY(ReplicatedUsing = OnRep_Damaged)
	float LastTimeDamageTaken;
	
	/**
	 * \brief Used to track if the enemy has self destructed.
	 */
	bool bSelfDestruct;
	
	/**
	 * \brief The next location the enemy should move to.
	 */
	FVector NextPathPoint;

	/**
	 * \brief Instance of the mesh's material, required to make changes to actor instance at run time.
	 */
	UMaterialInstanceDynamic* MaterialInstance;

	/**
	 * \brief Handle used to manage the self destruct timer.
	 */
	FTimerHandle TimerHandle_SelfDestruct;

	/**
	 * \brief Name of the parameter used to pulse material when taking damage. (Defined in M_ExplodingEnemy)
	 */
	const FName MaterialInstanceDamageParameterName = "LastTimeDamageTaken";
};