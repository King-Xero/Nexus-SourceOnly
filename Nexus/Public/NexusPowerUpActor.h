// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexusPowerUpActor.generated.h"

class UPointLightComponent;
class URotatingMovementComponent;

UCLASS()
class NEXUS_API ANexusPowerUpActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANexusPowerUpActor();
		
	/**
	 * \brief Start processing the power up.
	 * \param ActivatingActor The actor that activated the power up.
	 */
	void ActivatePowerUp(AActor* ActivatingActor);

	/**
	 * \brief Stop processing the power up.
	 */
	void DeactivatePowerUp();
	
	/**
	 * \brief Hook to implement power up activation in blueprint.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
	void OnActivated();

	/**
	 * \brief Hook to implement power up deactivation in blueprint.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
	void OnDeactivated();

	/**
	 * \brief Hook to implement power up effect to be applied in blueprint.
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
	 * \brief Timer callback used to float the power up.
	 */
	UFUNCTION()
	void Float();

	/**
	 * \brief Replicate power up activation.
	 */
	UFUNCTION()
	void OnRep_PowerUpActivated();

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
	 * \brief Used to rotate the power up.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	URotatingMovementComponent* RotatingComponent;
	
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

	/**
	 * \brief Interval between float updates.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "PowerUps")
	float FloatInterval = 0.016f;

	/**
	 * \brief Scale factor for floating height change.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "PowerUps")
	float FloatHeightChangeScale = 20.0f;

	/**
	 * \brief Cached actor that activated the power up.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "PowerUps")
	AActor* PowerUpActivatingActor;

private:

	/**
	 * \brief Method to execute functionality for power up activation.
	 * \param bActive Activation state of the power up.
	 */
	void OnPowerUpStateChanged(bool bActive);
	
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
	 * \brief Handle for the timer used to "float" the power up;
	 */
	FTimerHandle TimerHandle_Float;

	/**
	 * \brief Used to track and replicate the power up activation.
	 */
	UPROPERTY(ReplicatedUsing = OnRep_PowerUpActivated)
	bool bPowerUpActivated;

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
