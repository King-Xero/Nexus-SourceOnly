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
	 * \brief The type of damage that the weapon inflicts.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;
	
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

private:
	
	/**
	 * \brief Range within which the weapon can hit a target.
	 */
	float WeaponRange = 10000;

	/**
	 * \brief The amount of damage this weapon deals.
	 */
	float WeaponDamage = 20.0f;
};
