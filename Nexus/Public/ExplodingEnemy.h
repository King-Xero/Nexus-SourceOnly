// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ExplodingEnemy.generated.h"

class UNexusHealthComponent;

UCLASS()
class NEXUS_API AExplodingEnemy : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AExplodingEnemy();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

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
	 * \brief The visible mesh of the barrel.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	/**
	 * \brief Component used to manage health.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNexusHealthComponent* EnemyHealthComponent;

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

private:
	/**
	 * \brief The next location the enemy should move to.
	 */
	FVector NextPathPoint;

	/**
	 * \brief Used to track if the character is dead.
	 */
	bool bDead;

	/**
	 * \brief Instance of the mesh's material, required to make changes to actor instance at run time.
	 */
	UMaterialInstanceDynamic* MaterialInstance;

	/**
	 * \brief Name of the parameter used to pulse material when taking damage. (Defined in M_ExplodingEnemy)
	 */
	const FName MaterialInstanceDamageParameterName = "LastTimeDamageTaken";
};