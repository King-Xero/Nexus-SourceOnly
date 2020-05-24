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

protected:

	/**
	 * \brief Projectile class to spawn when weapon is fired.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AGrenadeLauncherProjectile> ProjectileClass;

	/**
	 * \brief The amount of damage this weapon deals.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float DamageRadius;
	
	virtual void Fire() override;
};
