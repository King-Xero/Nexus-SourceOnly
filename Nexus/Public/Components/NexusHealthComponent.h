// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NexusHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, UNexusHealthComponent*, HealthComponent, float, Health, float, HealthDelta, const UDamageType*, DamageType, AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(Nexus), meta=(BlueprintSpawnableComponent) )
class NEXUS_API UNexusHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNexusHealthComponent();

	/**
	 * \brief Get the max health for the health component
	 * \return Value for max health
	 */
	UFUNCTION()
	const float& GetMaxHealth() const;

	/**
	 * \brief Get the current health for the health component.
	 * \return Value for current health.
	 */
	UFUNCTION()
	const float& GetCurrentHealth() const;

	/**
	 * \brief Decrease current health of the health component. Delegate method for owners OnTakeAnyDamage event. Uses the signature for FTakeAnyDamageSignature.
	 * \param DamagedActor The actor receiving the damage
	 * \param DamageAmount The amount of health to deplete.
	 * \param DamageType the type of damage that is being inflicted
	 * \param InstigatedBy The controller that is inflicting the damage (specific player or ai)
	 * \param DamageCauser The actor that inflicted the damage (player, weapon, etc)
	 */
	UFUNCTION()
	void TakeDamage(AActor* DamagedActor, float DamageAmount, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealthChanged;

protected:

	/**
	 * \brief The maximum amount of health available
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float MaxHealth = 100.0f;
	
	// Called when the game starts
	virtual void BeginPlay() override;

private:

	/**
	 * \brief The current amount of health available.
	 */
	UPROPERTY(VisibleAnywhere, Category = "Health")
	float CurrentHealth;		
};
