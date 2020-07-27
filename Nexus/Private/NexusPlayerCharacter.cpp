// Toyan Green © 2020

#include "NexusPlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "NexusHUDUserWidget.h"
#include "NexusGameState.h"

void ANexusPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass)
	{
		// Create the player's HUD.
		CurrentHUDWidget = CreateWidget<UNexusHUDUserWidget>(GetWorld(), HUDWidgetClass);

		if (CurrentHUDWidget)
		{
			// Add the HUD to the players screen.
			CurrentHUDWidget->AddToViewport();

			// Configure health variables and bind to health component for health updates.
			CurrentHUDWidget->ConfigureHealthAndBindToComponent(CharacterHealthComponent);

			// Bind to player for ADS updates.
			CurrentHUDWidget->BindToPlayer(this);
			
			// Configure weapon variables and bind to current weapon for ammo updates.
			CurrentHUDWidget->ConfigureAndBindWeapon(CurrentWeapon);

			// Bind to game state for wave updates.
			CurrentHUDWidget->BindToGameState(GetWorld()->GetGameState<ANexusGameState>());
		}
	}
}
