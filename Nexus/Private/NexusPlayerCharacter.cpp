// Toyan Green © 2020

#include "NexusPlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "NexusHUDUserWidget.h"
#include "NexusGameState.h"
#include "Kismet/GameplayStatics.h"

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

void ANexusPlayerCharacter::HealthChanged(UNexusHealthComponent* HealthComponent, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	Super::HealthChanged(HealthComponent, Health, HealthDelta, DamageType, InstigatedBy, DamageCauser);

	// Dead flag is set in super.
	if (IsDead())
	{
		// Remove the HUD from the players screen.
		CurrentHUDWidget->RemoveFromViewport();
		
		APlayerController* PlayerController = GetController<APlayerController>();

		// Find the game over view target actor.
		AActor* GameOverViewTarget = UGameplayStatics::GetActorOfClass(this, GameOverViewTargetClass);

		if (PlayerController && GameOverViewTarget)
		{
			// Change the players camera view to that of the GameOverViewTarget actor.
			PlayerController->SetViewTargetWithBlend(GameOverViewTarget, GameOverViewBlendTime, EViewTargetBlendFunction::VTBlend_Cubic);
		}

		// Stop the player from being destroyed so that camera doesn't shift.
		SetLifeSpan(0);
	}
}