// Toyan Green © 2020

#include "NexusPlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "NexusHUDUserWidget.h"
#include "NexusGameState.h"
#include "Kismet/GameplayStatics.h"
#include "NexusPlayerState.h"

void ANexusPlayerCharacter::SwapWeapon()
{
	// Call super for swap weapon functionality
	Super::SwapWeapon();

	// Configure weapon variables and bind to current weapon for ammo updates.
	CurrentHUDWidget->ConfigureAndBindWeapon(CurrentWeapon);
}

void ANexusPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetController<APlayerController>();

	if (PlayerController)
	{
		// Disable player input until the timer expires.
		DisableInput(PlayerController);

		// Find the game over view target actor.
		AActor* GameOverViewTarget = UGameplayStatics::GetActorOfClass(this, GameOverViewTargetClass);

		if (GameOverViewTarget)
		{
			// Change the players camera view to that of the GameOverViewTarget actor.
			PlayerController->SetViewTarget(GameOverViewTarget);
			// Blend into the player camera view.
			PlayerController->SetViewTargetWithBlend(this, GameOverViewBlendTime,
			                                         EViewTargetBlendFunction::VTBlend_Cubic);
		}
	}
	
	if (HUDWidgetClass)
	{
		// Create the player's HUD.
		CurrentHUDWidget = CreateWidget<UNexusHUDUserWidget>(GetWorld(), HUDWidgetClass);

		if (CurrentHUDWidget)
		{
			// Configure health variables and bind to health component for health updates.
			CurrentHUDWidget->ConfigureHealthAndBindToComponent(CharacterHealthComponent);

			// Bind to player for ADS updates.
			CurrentHUDWidget->BindToPlayer(this);
			
			// Configure weapon variables and bind to current weapon for ammo updates.
			CurrentHUDWidget->ConfigureAndBindWeapon(CurrentWeapon);

			// Bind to game state for wave updates.
			CurrentHUDWidget->BindToGameState(GetWorld()->GetGameState<ANexusGameState>());

			// Bind to player state for score updates.
			ANexusPlayerState* PState = GetPlayerStateChecked<ANexusPlayerState>();
			if(PState)
			{
				CurrentHUDWidget->BindToPlayerState(PState);
			}			
		}
	}

	// Start timer that will enable player input and add the hud to the screen after the camera has transitioned.
	GetWorldTimerManager().SetTimer(TimerHandle_PlayerStart, this, &ANexusPlayerCharacter::DelayedPlayerStart, GameOverViewBlendTime);
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

void ANexusPlayerCharacter::DelayedPlayerStart()
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController)
	{
		// Enable player input.
		EnableInput(PlayerController);
	}

	if (CurrentHUDWidget)
	{
		// Add the HUD to the players screen.
		CurrentHUDWidget->AddToViewport();
	}
}
