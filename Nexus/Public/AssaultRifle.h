// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "NexusWeapon.h"
#include "AssaultRifle.generated.h"

/**
 * 
 */
UCLASS()
class NEXUS_API AAssaultRifle : public ANexusWeapon
{
	GENERATED_BODY()

public:

	/**
	 * \brief Start shooting the weapon.
	 */
	virtual void StartFiring() override;

protected:

	/**
	 * \brief Shoot the weapon.
	 */
	virtual void Fire() override;
	
};
