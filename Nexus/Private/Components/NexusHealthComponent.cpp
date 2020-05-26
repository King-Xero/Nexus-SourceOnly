// Toyan Green © 2020


#include "Components/NexusHealthComponent.h"
#include "Nexus/Utils/Logging/NexusLogging.h"

// Sets default values for this component's properties
UNexusHealthComponent::UNexusHealthComponent()
{
	// The health component should be reactive, so does not need to update every frame.
	PrimaryComponentTick.bCanEverTick = false;
}

const float& UNexusHealthComponent::GetMaxHealth() const
{
	return MaxHealth;
}

const float& UNexusHealthComponent::GetCurrentHealth() const
{
	return CurrentHealth;
}

// Called when the game starts
void UNexusHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// Wire up health component to owners OnTakeAnyDamage event.
	AActor* cOwner = GetOwner();
	if (cOwner)
	{
		cOwner->OnTakeAnyDamage.AddDynamic(this, &UNexusHealthComponent::TakeDamage);
	}

	// Initialise current health
	CurrentHealth = MaxHealth;
}

void UNexusHealthComponent::TakeDamage(AActor* DamagedActor, float DamageAmount, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	// Only deplete health if the damage amount is a positive value.
	if (0 > DamageAmount)
	{
		checkf(false, TEXT("%hs called with invalid value: %f"), __FUNCTION__, DamageAmount);
	}
	else
	{
		FStringFormatOrderedArguments LogArgs;
		LogArgs.Add(FStringFormatArg(DamageAmount));
		FNexusLogging::Log(ELogLevel::DEBUG, FString::Format(TEXT("Damage inflicted: {0}"), LogArgs));
		
		// Ensure that the new health value is between 0 and max health.
		CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);

		// Raise the health changed event.
		OnHealthChanged.Broadcast(this, CurrentHealth, DamageAmount, DamageType, InstigatedBy, DamageCauser);
	}
}