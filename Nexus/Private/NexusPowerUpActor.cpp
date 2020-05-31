// Toyan Green © 2020


#include "NexusPowerUpActor.h"

// Sets default values
ANexusPowerUpActor::ANexusPowerUpActor()
{
 	// Power up does not need to tick.
	PrimaryActorTick.bCanEverTick = false;

	TicksProcessed = 0;
}

void ANexusPowerUpActor::ActivatePowerUp()
{
	// If the effect is applied more than once, we should set a timer to repeatedly apply it.
	if (TotalNumberOfTicks > 1)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerUp, this, &ANexusPowerUpActor::PowerUpTick, PowerUpInterval, true, 0.0f);
	}
	else
	{
		// Apply the effect once.
		PowerUpTick();
	}
}

// Called when the game starts or when spawned
void ANexusPowerUpActor::BeginPlay()
{
	Super::BeginPlay();	
}

void ANexusPowerUpActor::PowerUpTick()
{
	// Apply the power up effect.
	ApplyPowerUpEffect();
	
	if (++TicksProcessed >= TotalNumberOfTicks)
	{
		// The power up has expired, so deactivate the effect.
		OnDeactivated();

		// Clear the timer so we stop processing the power up.
		GetWorldTimerManager().ClearTimer(TimerHandle_PowerUp);
	}	
}