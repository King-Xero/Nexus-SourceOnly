// Toyan Green © 2020


#include "NexusHUDUserWidget.h"
#include "Components/NexusHealthComponent.h"
#include "NexusWeapon.h"
#include "NexusPlayerCharacter.h"

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
