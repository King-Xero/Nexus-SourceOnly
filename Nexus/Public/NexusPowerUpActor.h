// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexusPowerUpActor.generated.h"

class UPointLightComponent;

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
	 * \brief The visible mesh of the power up.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	/**
	 * \brief The light surrounding the power up.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPointLightComponent* LightComponent;
	
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

	/**
	 * \brief Color of the power.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "PowerUps")
	FLinearColor PowerUpColor = FLinearColor::White;
	
	/**
	 * \brief Multiplier used to adjust the speed of the power up flash.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "PowerUps")
	float PowerUpFlashTimerMultiplier = 0.6f;

	/**
	 * \brief Minimum emissive value of the power up flash.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "PowerUps")
	float PowerUpFlashMinimumEmissive = 0.2f;

private:

	/**
	 * \brief Instance of the mesh's material, required to make changes to material instance at run time.
	 */
	UMaterialInstanceDynamic* PowerUpMaterialInstance;

	/**
	 * \brief Instance of the light's material, required to make changes to material instance at run time.
	 */
	UMaterialInstanceDynamic* LightMaterialInstance;
	
	/**
	 * \brief Timer used to apply power up effect when PowerUpInterval > 0;
	 */
	FTimerHandle TimerHandle_PowerUp;

	// Used to track how many times the power up effect has been applied.
	int TicksProcessed;

	/**
	 * \brief Name of parameter used to alter the color of the power up.
	 */
	FName MaterialInstanceColorParameterName = "PowerUpColor";
	
	/**
	 * \brief Name of the parameter used to alter the speed of the power up flash.
	 */
	FName MaterialInstanceTimeMultiplierParameterName = "TimeMultiplier";
	
	/**
	 * \brief Name of the parameter used to set the minimum emissive value of the power up flash.
	 */
	FName MaterialInstanceMinimumEmissiveParameterName = "MinimumEmissiveValue";
};
