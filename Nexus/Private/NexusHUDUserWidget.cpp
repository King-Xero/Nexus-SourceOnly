// Toyan Green © 2020


#include "NexusHUDUserWidget.h"
#include "Components/NexusHealthComponent.h"
#include "NexusWeapon.h"
#include "NexusPlayerCharacter.h"
#include "NexusGameState.h"
#include "NexusPlayerState.h"
#include "Sound/SoundCue.h"

void UNexusHUDUserWidget::ConfigureHealthAndBindToComponent(UNexusHealthComponent* HealthComponent)
{
	SetHealthAndPublishChange(HealthComponent->GetCurrentHealth(), HealthComponent->GetMaxHealth());
	SetArmourAndPublishChange(HealthComponent->GetCurrentArmour(), HealthComponent->GetMaxArmour());
	
	HealthComponent->OnCurrentHealthUpdated.AddDynamic(this, &UNexusHUDUserWidget::HealthChanged);
}

void UNexusHUDUserWidget::ConfigureAndBindWeapon(ANexusWeapon* Weapon)
{
	// Should not rebind to the same weapon
	if (CurrentWeapon != Weapon)
	{
		if (CurrentWeapon)
		{
			// The UI should no longer listen for updates from the old weapon.
			CurrentWeapon->OnAmmoUpdated.RemoveAll(this);
		}

		CurrentWeapon = Weapon;
		
		SetAmmoAndPublishChange(Weapon->GetAmmoInClip(), Weapon->GetAmmoInReserve());

		HipFireCrosshair = Weapon->GetHipFireCrosshairTexture();
		ADSCrosshair = Weapon->GetADSCrosshairTexture();

		SetWeaponNameAndPublishChange(Weapon->GetWeaponName());

		SetActiveCrosshairAndPublishChange(bAimingDownSight);

		Weapon->OnAmmoUpdated.AddDynamic(this, &UNexusHUDUserWidget::AmmoChanged);
	}	
}

void UNexusHUDUserWidget::BindToPlayer(ANexusPlayerCharacter* Player)
{
	Player->OnADSUpdated.AddDynamic(this, &UNexusHUDUserWidget::ADSChanged);

	SetActiveCrosshairAndPublishChange(Player->IsAimingDownSights());
}

void UNexusHUDUserWidget::BindToGameState(ANexusGameState* GameState)
{
	GameState->OnWaveStateUpdated.AddDynamic(this, &UNexusHUDUserWidget::WaveStateChanged);
	GameState->OnWaveNumberUpdated.AddDynamic(this, &UNexusHUDUserWidget::WaveNumberChanged);
}

void UNexusHUDUserWidget::BindToPlayerState(ANexusPlayerState* PlayerState)
{
	SetScoreAndPublishChange(PlayerState->GetScore());
	PlayerState->OnScoreUpdated.AddDynamic(this, &UNexusHUDUserWidget::ScoreChanged);
}

void UNexusHUDUserWidget::HealthChanged(UNexusHealthComponent* HealthComponent)
{
	// Update health.
	SetHealthAndPublishChange(HealthComponent->GetCurrentHealth(), HealthComponent->GetMaxHealth());
	// Update armour.
	SetArmourAndPublishChange(HealthComponent->GetCurrentArmour(), HealthComponent->GetMaxArmour());
}

void UNexusHUDUserWidget::AmmoChanged(ANexusWeapon* Weapon, int32 NewAmmoInClip, int32 NewAmmoInReserve)
{
	SetAmmoAndPublishChange(NewAmmoInClip, NewAmmoInReserve);
}

void UNexusHUDUserWidget::ADSChanged(ANexusCharacter* Character, bool bIsPlayerAiming)
{
	bAimingDownSight = bIsPlayerAiming;

	SetActiveCrosshairAndPublishChange(bIsPlayerAiming);
}

void UNexusHUDUserWidget::WaveStateChanged(ANexusGameState* GameState, EWaveState OldState, EWaveState NewState)
{
	SetWaveStateAndPublishChange(OldState, NewState);
}

void UNexusHUDUserWidget::WaveNumberChanged(ANexusGameState* GameState, int WaveNumber)
{
	SetWaveNumberAndPublishChange(WaveNumber);
}

void UNexusHUDUserWidget::ScoreChanged(ANexusPlayerState* PlayerState, float NewScore)
{
	SetScoreAndPublishChange(NewScore);
}

void UNexusHUDUserWidget::SetHealthAndPublishChange(float NewCurrentHealth, float NewMaxHealth)
{
	CurrentHealth = NewCurrentHealth;
	MaxHealth = NewMaxHealth;

	const float HealthPercentage = FMath::Clamp(CurrentHealth / MaxHealth, 0.0f, 1.0f);
	
	PublishHealthChanged(HealthPercentage);
}

void UNexusHUDUserWidget::SetArmourAndPublishChange(float NewCurrentArmour, float NewMaxArmour)
{
	CurrentArmour = NewCurrentArmour;
	MaxArmour = NewMaxArmour;

	const float ArmourPercentage = FMath::Clamp(CurrentArmour / MaxArmour, 0.0f, 1.0f);

	PublishArmourChanged(ArmourPercentage);
}

void UNexusHUDUserWidget::SetAmmoAndPublishChange(int32 NewAmmoInClip, int32 NewAmmoInReserve)
{
	AmmoInClip = NewAmmoInClip;
	AmmoInReserve = NewAmmoInReserve;

	const FString AmmoInClipString = FString::Printf(TEXT("%03d"), AmmoInClip);
	const FString AmmoInReserveString = FString::Printf(TEXT("%03d"), AmmoInReserve);
	
	PublishAmmoChanged(AmmoInClipString, AmmoInReserveString);
}

void UNexusHUDUserWidget::SetWeaponNameAndPublishChange(FName NewWeaponName)
{
	WeaponName = NewWeaponName;

	PublishWeaponNameChanged(WeaponName);
}

void UNexusHUDUserWidget::SetActiveCrosshairAndPublishChange(bool bIsPlayerAiming)
{
	ActiveCrosshair = bIsPlayerAiming ? ADSCrosshair : HipFireCrosshair;

	PublishCrosshairChanged(ActiveCrosshair);
}

void UNexusHUDUserWidget::SetWaveStateAndPublishChange(EWaveState OldState, EWaveState NewState)
{
	OldWaveState = OldState;
	NewWaveState = NewState;

	auto GameState = GetWorld()->GetGameState<ANexusGameState>();
	if (GameState && !GameState->IsGameOver())
	{
		const FString WaveNotificationText = GetWaveNotificationText(OldState, NewState);

		if (!WaveNotificationText.IsEmpty())
		{
			PublishWaveStateNotification(WaveNotificationText);
		}

		USoundCue* WaveAnnouncementSFX = GetWaveAnnouncementSFX(NewState);
		if (WaveAnnouncementSFX)
		{
			PlaySound(WaveAnnouncementSFX);
		}
	}	
}

void UNexusHUDUserWidget::SetWaveNumberAndPublishChange(int WaveNumber)
{
	CurrentWaveNumber = WaveNumber;

	PublishWaveNumberChanged(CurrentWaveNumber);
}

void UNexusHUDUserWidget::SetScoreAndPublishChange(float NewScore)
{
	CurrentScore = NewScore;

	PublishScoreChanged(CurrentScore);
}

FString UNexusHUDUserWidget::GetWaveNotificationText(EWaveState OldState, EWaveState NewState)
{
	switch (NewState)
	{
		case EWaveState::PreparingNextWave:
			return 1 >= CurrentWaveNumber ? PrepareForFirstWaveText : PrepareForNextWaveText;
		case EWaveState::WaveInProgress:
			return WaveStartingText;
		case EWaveState::WaveComplete:
			return WaveCompleteText;
		case EWaveState::GameOver:
		case EWaveState::WaitingToComplete:
		case EWaveState::Unknown:
		default:
			// No notification to display.
			return FString();
	}
	return FString();
}

USoundCue* UNexusHUDUserWidget::GetWaveAnnouncementSFX(EWaveState NewState)
{
	switch (NewState)
	{
		case EWaveState::PreparingNextWave:
			return 1 >= CurrentWaveNumber ? PrepareForFirstWaveAnnouncementSFX : PrepareForNextWaveAnnouncementSFX;
		case EWaveState::WaveInProgress:
			return WaveStartingAnnouncementSFX;
		case EWaveState::WaveComplete:
			return WaveCompleteAnnouncementSFX;
		case EWaveState::GameOver:
		case EWaveState::WaitingToComplete:
		case EWaveState::Unknown:
		default:
			// No sfx to play.
			return nullptr;
	}
}
