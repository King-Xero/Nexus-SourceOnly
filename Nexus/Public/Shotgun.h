// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "NexusWeapon.h"
#include "Shotgun.generated.h"

/**
 * 
 */
UCLASS()
class NEXUS_API AShotgun : public ANexusWeapon
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

	/**
	 * \brief The number of pellets fired per shot.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (ClampMin = 1))
	int NumberOfPelletsInShot = 12;
};
