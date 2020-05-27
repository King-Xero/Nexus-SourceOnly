// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrenadeLauncherProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class NEXUS_API AGrenadeLauncherProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrenadeLauncherProjectile();

	/**
	 * \brief Called when projectile hits something.
	 */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/**
	 * \brief Called when the lifespan of an actor expires (if it has one).
	 */
	virtual void LifeSpanExpired() override;

	void SetDamageAmount(float Damage);
	
	void SetDamageRadius(float Radius);

	void SetDamageType(TSubclassOf<UDamageType> Type);
	
protected:

	/**
	 * \brief Sphere collision component
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	USphereComponent* CollisionComponent;

	/**
	 * \brief Projectile movement component.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	UProjectileMovementComponent* ProjectileMovement;

	/**
	 * \brief Magnitude of the impulse when projectile hits an actor that is simulating physics.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float PhysicsImpulseStrength = 50.0f;

	/**
	 * \brief Particle effect spawned at the projectile's location when it explodes.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* ExplosionVFX;
	
	/**
	 * \brief Explode the projectile.
	 */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Explode();

	/**
	 * \brief Spawn particle effect for explosion.
	 */
	void PlayExplosionEffect() const;
	
private:

	/**
	 * \brief The amount of damage this weapon deals.
	 */
	float DamageAmount;

	/**
	 * \brief The radius of the radial damage this weapon deals.
	 */
	float DamageRadius;

	/**
	 * \brief The type of damage that the weapon inflicts.
	 */
	TSubclassOf<UDamageType> DamageType;
};