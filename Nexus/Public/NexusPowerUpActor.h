// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexusPowerUpActor.generated.h"

UCLASS()
class NEXUS_API ANexusPowerUpActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANexusPowerUpActor();
		
	/**
	 * \brief Start processing the power up.
	 */
	void ActivatePowerUp();
	
	/**
	 * \brief Activate the power up.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
	void OnActivated();

	/**
	 * \brief Deactivate the power up.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
	void OnDeactivated();

	/**
	 * \brief Apply the power up effect.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
	void ApplyPowerUpEffect();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/**
	 * \brief Timer callback used to apply the power up effect.
	 */
	UFUNCTION()
	void PowerUpTick();
	
	/**
	 * \brief Interval between power up ticks.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "PowerUps")
	float PowerUpInterval = 1.0f;
	
	/**
	 * \brief Total number of time the effect is applied.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "PowerUps")
	int TotalNumberOfTicks = 1;

private:
	
	/**
	 * \brief Timer used to apply power up effect when PowerUpInterval > 0;
	 */
	FTimerHandle TimerHandle_PowerUp;

	// Used to track how many times the power up effect has been applied.
	int TicksProcessed;
};
