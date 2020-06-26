// Toyan Green © 2020


#include "Components/NexusHealthComponent.h"
#include "Nexus/Utils/Logging/NexusLogging.h"
#include "Net/UnrealNetwork.h"
#include "NexusGameModeBase.h"

// Sets default values for this component's properties
UNexusHealthComponent::UNexusHealthComponent()
{
	// The health component should be reactive, so does not need to update every frame.
	PrimaryComponentTick.bCanEverTick = false;

	// Needs to be set to that the component is replicated on all clients.
	SetIsReplicatedByDefault(true);
}

void UNexusHealthComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate health values on all clients.
	DOREPLIFETIME(UNexusHealthComponent, MaxHealth);
	DOREPLIFETIME(UNexusHealthComponent, CurrentHealth);
}


const float& UNexusHealthComponent::GetMaxHealth() const
{
	return MaxHealth;
}

const float& UNexusHealthComponent::GetCurrentHealth() const
{
	return CurrentHealth;
}

bool UNexusHealthComponent::IsFriendly(AActor* Actor1, AActor* Actor2)
{
	if (!Actor1 || !Actor2)
	{
		// If we can't identify actors, assume they are not friendly.
		return false;
	}
	UNexusHealthComponent* HealthComponent1 = Cast<UNexusHealthComponent>(Actor1->GetComponentByClass(UNexusHealthComponent::StaticClass()));
	UNexusHealthComponent* HealthComponent2 = Cast<UNexusHealthComponent>(Actor2->GetComponentByClass(UNexusHealthComponent::StaticClass()));

	if (!HealthComponent1 || !HealthComponent2)
	{
		// If we can't identify actors, assume they are not friendly.
		return false;
	}

	// If team numbers are equal, actors are actually friendly.
	return HealthComponent1->TeamID == HealthComponent2->TeamID;
}

// Called when the game starts
void UNexusHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// Only wire up the damage event on the server.
	if (ROLE_Authority == GetOwnerRole())
	{
		// Wire up health component to owners OnTakeAnyDamage event.
		AActor* cOwner = GetOwner();
		if (cOwner)
		{
			cOwner->OnTakeAnyDamage.AddDynamic(this, &UNexusHealthComponent::TakeDamage);
		}
	}	

	// Initialise current health
	CurrentHealth = MaxHealth;
}

void UNexusHealthComponent::TakeDamage(AActor* DamagedActor, float DamageAmount, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	AActor* InstigatingActor;

	if(InstigatedBy && InstigatedBy->GetPawn())
	{
		InstigatingActor = InstigatedBy->GetPawn();
	}
	else
	{
		InstigatingActor = DamageCauser;
	}
	
	// Self damage is allowed, friendly fire is not.
	if(DamagedActor == InstigatingActor || !IsFriendly(DamagedActor, InstigatingActor))
	{
		// Ensure that health only changes whilst the owner still has health and is alive.
		if (0.0f < CurrentHealth && !bDead)
		{
			FStringFormatOrderedArguments LogArgs;
			LogArgs.Add(FStringFormatArg(DamageAmount));

			if (0.0f < DamageAmount)
			{
				// If DamageAmount is positive, health is depleted. 
				FNexusLogging::Log(ELogLevel::DEBUG, FString::Format(TEXT("Health depleted: {0}"), LogArgs));
			}
			else
			{
				// If DamageAmount is negative, health is replenished.
				FNexusLogging::Log(ELogLevel::DEBUG, FString::Format(TEXT("Health replenished: {0}"), LogArgs));
			}

			// Ensure that the new health value is between 0 and max health.
			CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);

			// Check if the owner is dead.
			bDead = 0.0f >= CurrentHealth;

			// Broadcast health update locally.
			OnRep_CurrentHealthUpdated();

			if (bDead)
			{
				// If the actor has died, we should inform the game mode.
				ANexusGameModeBase* GameMode = Cast<ANexusGameModeBase>(GetWorld()->GetAuthGameMode());
				if (GameMode)
				{
					// The owner has been killed by the damage causer.
					GameMode->OnActorKilled.Broadcast(GetOwner(), InstigatedBy, DamageCauser);
				}
			}

			// Raise the health changed event.
			OnHealthChanged.Broadcast(this, CurrentHealth, DamageAmount, DamageType, InstigatedBy, DamageCauser);
		}
		else
		{
			FNexusLogging::Log(ELogLevel::DEBUG, TEXT("Component owner is already dead."));
		}
	}
}

void UNexusHealthComponent::OnRep_CurrentHealthUpdated() const
{
	// Broadcast current health update to replicated clients.
	OnCurrentHealthUpdated.Broadcast();
}
