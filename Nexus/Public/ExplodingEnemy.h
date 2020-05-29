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
	 * \brief The visible mesh of the barrel.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

};