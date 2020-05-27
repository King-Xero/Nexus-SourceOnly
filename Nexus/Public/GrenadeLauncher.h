// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "NexusWeapon.h"
#include "GrenadeLauncherProjectile.h"
#include "GrenadeLauncher.generated.h"

/**
 * 
 */
UCLASS()
class NEXUS_API AGrenadeLauncher : public ANexusWeapon
{
	GENERATED_BODY()

public:

	/**
	 * \brief Start shooting the weapon.
	 */
	virtual void StartFiring() override;
	
protected:

	/**
	 * \brief Projectile class to spawn when weapon is fired.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AGrenadeLauncherProjectile> ProjectileClass;

	/**
	 * \brief The distance within which this projectile deals damage.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float DamageRadius;

	/**
	 * \brief Shoot the weapon.
	 */
	virtual void Fire() override;
};
