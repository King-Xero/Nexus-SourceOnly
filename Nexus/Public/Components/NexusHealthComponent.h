// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NexusHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, UNexusHealthComponent*, HealthComponent, float, Health, float, HealthDelta, const UDamageType*, DamageType, AController*, InstigatedBy, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCurrentHealthReplicatedUpdateEvent);

UCLASS( ClassGroup=(Nexus), meta=(BlueprintSpawnableComponent) )
class NEXUS_API UNexusHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNexusHealthComponent();

	/**
	 * \brief Get the max health for the health component.
	 * \return Value for max health.
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
	 * \brief Event used to broadcast detailed health updates.
	 */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealthChanged;

	/**
	 * \brief Event used to broadcast current health updates on replicated clients.
	 */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FCurrentHealthReplicatedUpdateEvent OnCurrentHealthUpdated;

protected:

	// Called when the game starts
	virtual void BeginPlay() override;

	/**
	 * \brief Alter current health of the health component. <b>**Negative damage is used to replenish health**</b> Delegate method for owner's OnTakeAnyDamage event. Uses the signature for FTakeAnyDamageSignature.
	 * \param DamagedActor The actor receiving the damage.
	 * \param DamageAmount The amount of health to deplete.
	 * \param DamageType The type of damage that is being inflicted.
	 * \param InstigatedBy The controller that is inflicting the damage. (specific player or ai)
	 * \param DamageCauser The actor that inflicted the damage. (player, weapon, etc)
	 *	@note Negative damage is used to replenish health.
	 */
	UFUNCTION()
	void TakeDamage(AActor* DamagedActor, float DamageAmount, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	/**
	 * \brief The maximum amount of health available.
	 */
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float MaxHealth = 100.0f;

	/**
	 * \brief The current amount of health available.
	 */
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealthUpdated, BlueprintReadOnly, VisibleAnywhere, Category = "Health")
	float CurrentHealth;

private:

	/**
	 * \brief Used to track if the owner is dead.
	 */
	bool bDead = false;

	/**
	 * \brief Replicate current health updates.
	 */
	UFUNCTION()
	void OnRep_CurrentHealthUpdated() const;	
};
