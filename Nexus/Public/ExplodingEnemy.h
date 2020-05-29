// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ExplodingEnemy.generated.h"

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
	 * \brief The visible mesh of the barrel.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

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

};