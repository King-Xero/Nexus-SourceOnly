// Toyan Green © 2020


#include "NexusHUDUserWidget.h"
#include "Components/NexusHealthComponent.h"
#include "NexusWeapon.h"
#include "NexusPlayerCharacter.h"
#include "NexusGameState.h"

void UNexusHUDUserWidget::ConfigureHealthAndBindToComponent(UNexusHealthComponent* HealthComponent)
{
	SetHealthAndPublishChange(HealthComponent->GetCurrentHealth(), HealthComponent->GetMaxHealth());
	
	HealthComponent->OnHealthChanged.AddDynamic(this, &UNexusHUDUserWidget::HealthChanged);
}

void UNexusHUDUserWidget::ConfigureAndBindWeapon(ANexusWeapon* Weapon)
{
	SetAmmoAndPublishChange(Weapon->GetAmmoInClip(), Weapon->GetAmmoInReserve());
	
	HipFireCrosshair = Weapon->GetHipFireCrosshairTexture();
	ADSCrosshair = Weapon->GetADSCrosshairTexture();

	SetWeaponNameAndPublishChange(Weapon->GetWeaponName());

	SetActiveCrosshairAndPublishChange(bAimingDownSight);

	Weapon->OnAmmoUpdated.AddDynamic(this, &UNexusHUDUserWidget::AmmoChanged);
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

void UNexusHUDUserWidget::HealthChanged(UNexusHealthComponent* HealthComponent, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	SetHealthAndPublishChange(Health, HealthComponent->GetMaxHealth());
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

void UNexusHUDUserWidget::WaveNumberChanged(ANexusGameState* GameState, uint8 WaveNumber)
{
	SetWaveNumberAndPublishChange(WaveNumber);
}

void UNexusHUDUserWidget::SetHealthAndPublishChange(float NewCurrentHealth, float NewMaxHealth)
{
	CurrentHealth = NewCurrentHealth;
	MaxHealth = NewMaxHealth;

	const float HealthPercentage = FMath::Clamp(CurrentHealth / MaxHealth, 0.0f, 1.0f);
	
	PublishHealthChanged(HealthPercentage);
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

	const FString WaveNotificationText = GetWaveNotificationText(OldState, NewState);

	if(!WaveNotificationText.IsEmpty())
	{
		PublishWaveStateNotification(WaveNotificationText);
	}	
}

void UNexusHUDUserWidget::SetWaveNumberAndPublishChange(uint8 WaveNumber)
{
	CurrentWaveNumber = WaveNumber;

	PublishWaveNumberChanged(CurrentWaveNumber);
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
