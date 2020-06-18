// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplodingBarrel.generated.h"

class UNexusHealthComponent;
class URadialForceComponent;

UCLASS()
class NEXUS_API AExplodingBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplodingBarrel();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

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
	 * \brief Explode the barrel.
	 */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Explode();

	/**
	 * \brief Replicate barrel explosion effects.
	 */
	UFUNCTION()
	void OnRep_Explode() const;
	
	/**
	 * \brief The visible mesh of the barrel.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	/**
	 * \brief Component used to manage health.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNexusHealthComponent* BarrelHealthComponent;

	/**
	 * \brief Component used to emit radial force when the barrel explodes.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	URadialForceComponent* RadialForceComponent;

	/**
	 * \brief Particle effect spawned at the barrel's location when it explodes.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barrel")
	UParticleSystem* ExplosionVFX;

	/**
	 * \brief The material that should be applied to the barrel when it explodes.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barrel")
	UMaterialInterface* ExplodedMaterial;

	/**
	 * \brief Sound effect spawned at the barrel's location when it explodes.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barrel")
	USoundBase* ExplosionSFX;

	/**
	 * \brief Magnitude of the vertical impulse when the barrel is launched up as it explodes.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barrel")
	float VerticalImpulseStrength = 500.0f;

	/**
	 * \brief Magnitude of the radial impulse emitted from the barrel when it explodes.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barrel")
	float RadialImpulseStrength = 500.0f;

	/**
	 * \brief The amount of damage dealt when the barrel explodes.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barrel")
	float ExplosionDamage = 20.0f;

	/**
	 * \brief The distance within which this barrel deals damage.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barrel")
	float ExplosionRadius = 200.0f;

	/**
	 * \brief The number of seconds until the barrel is destroyed, after the barrel explodes. (0 = barrel will not be destroyed)
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barrel")
	float ExplosionLifeSpan = 10.0f;

	/**
	 * \brief The type of damage that the barrel inflicts.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barrel")
	TSubclassOf<UDamageType> BarrelDamageType;

private:
		
	/**
	 * \brief Spawn particle effect for explosion.
	 */
	void PlayExplosionVFX() const;

	/**
	 * \brief Spawn sound effect for explosion.
	 */
	void PlayExplosionSFX() const;

	/**
	 * \brief Swap the mesh material to the exploded material.
	 */
	void SetExplodedMaterial() const;

	/**
	 * \brief Used to track if the barrel has exploded.
	 */
	UPROPERTY(ReplicatedUsing = OnRep_Explode)
	bool bExploded;
};