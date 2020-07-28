// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NexusHUDUserWidget.generated.h"

enum class EWaveState : unsigned char;
class ANexusGameState;
class ANexusCharacter;
class ANexusPlayerCharacter;
class ANexusWeapon;
class UNexusHealthComponent;

UCLASS()
class NEXUS_API UNexusHUDUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	/**
	 * \brief Setup health variables and bind to health component for health updates.
	 * \param HealthComponent 
	 */
	void ConfigureHealthAndBindToComponent(UNexusHealthComponent* HealthComponent);

	/**
	 * \brief Setup weapon variables and bind to weapon for ammo updates.
	 * \param Weapon 
	 */
	void ConfigureAndBindWeapon(ANexusWeapon* Weapon);

	/**
	 * \brief Bind to player for ADS updates.
	 * \param Player 
	 */
	void BindToPlayer(ANexusPlayerCharacter* Player);

	/**
	 * \brief Bind to game state for wave updates.
	 * \param GameState
	 */
	void BindToGameState(ANexusGameState* GameState);

protected:

	/**
	 * \brief Respond to a change in health. Wired up to health components OnHealthChanged event. Uses the signature for FOnHealthChangedSignature.
	 * \param HealthComponent The health component the experienced a health change.
	 * \param Health The amount of health the component now has.
	 * \param HealthDelta The amount of health that changed.
	 * \param DamageType The type of damage that was being inflicted.
	 * \param InstigatedBy The controller that inflicted the health change. (specific player or ai)
	 * \param DamageCauser The actor that inflicted the health change. (player, weapon, etc)
	 */
	UFUNCTION()
	void HealthChanged(UNexusHealthComponent* HealthComponent, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	/**
	 * \brief Respond to a change in ammo. Wired up to a weapons OnAmmoUpdated event.
	 * \param Weapon The weapon that experienced the change.
	 * \param NewAmmoInClip The new value for ammo in the clip.
	 * \param NewAmmoInReserve The new value to ammo in reserve.
	 */
	UFUNCTION()
	void AmmoChanged(ANexusWeapon* Weapon, int32 NewAmmoInClip, int32 NewAmmoInReserve);

	/**
	 * \brief Respond to the character changing their aiming. Wired up to characters OnADSUpdated event.
	 * \param Character The player character that is changing.
	 * \param bIsPlayerAiming Is the player aiming down sights.
	 */
	UFUNCTION()
	void ADSChanged(ANexusCharacter* Character, bool bIsPlayerAiming);

	/**
	 * \brief Respond to the wave state changing. Wired up to gamestates OnWaveStateUpdated event.
	 * \param GameState 
	 * \param OldState Wave state that is being exited.
	 * \param NewState Wave state that is being entered.
	 */
	UFUNCTION()
	void WaveStateChanged(ANexusGameState* GameState, EWaveState OldState, EWaveState NewState);

	/**
	 * \brief Respond to the wave number increasing. Wired up to gamestates OnWaveNumberUpdated event.
	 * \param GameState 
	 * \param WaveNumber The wave number to be displayed in the UI.
	 */
	UFUNCTION()
	void WaveNumberChanged(ANexusGameState* GameState, uint8 WaveNumber);

	/**
	 * \brief Hook to update health UI elements.
	 * \param CurrentHealthPercentage 
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void PublishHealthChanged(float CurrentHealthPercentage);

	/**
	 * \brief Hook to update ammo UI elements.
	 * \param AmmoInClipText 
	 * \param AmmoInReserveText 
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void PublishAmmoChanged(const FString& AmmoInClipText, const FString& AmmoInReserveText);

	/**
	 * \brief Hook to update weapon name UI element.
	 * \param WeaponNameText 
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void PublishWeaponNameChanged(FName WeaponNameText);

	/**
	 * \brief Hook to update the crosshair texture.
	 * \param CrosshairTexture 
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void PublishCrosshairChanged(UTexture* CrosshairTexture);

	/**
	 * \brief Hook to update the wave number UI element.
	 * \param CurrentWaveNumber 
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void PublishWaveNumberChanged(uint8 WaveNumber);

	/**
	 * \brief Hook to display wave notification.
	 * \param NotificationText 
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void PublishWaveStateNotification(const FString& NotificationText);

	/**
	 * \brief Text to display before first wave.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD")
	FString PrepareForFirstWaveText;
	
	/**
	 * \brief Text to display between waves.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD")
	FString PrepareForNextWaveText;

	/**
	 * \brief Text to display when a wave starts.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD")
	FString WaveStartingText;

	/**
	 * \brief Text to display when a wave is completed.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD")
	FString WaveCompleteText;
	
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	float CurrentHealth;
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	float MaxHealth;

	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	int32 AmmoInClip;
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	int32 AmmoInReserve;

	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	UTexture* ActiveCrosshair;
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	UTexture* HipFireCrosshair;
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	UTexture* ADSCrosshair;

	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	FName WeaponName;
	
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	bool bAimingDownSight;

	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	EWaveState OldWaveState;
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	EWaveState NewWaveState;

	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	uint8 CurrentWaveNumber;
	
private:

	/**
	 * \brief Set health variables and publish UI update.
	 * \param NewCurrentHealth 
	 * \param NewMaxHealth 
	 */
	void SetHealthAndPublishChange(float NewCurrentHealth, float NewMaxHealth);

	/**
	 * \brief Set ammo variables and publish UI update.
	 * \param NewAmmoInClip 
	 * \param NewAmmoInReserve 
	 */
	void SetAmmoAndPublishChange(int32 NewAmmoInClip, int32 NewAmmoInReserve);

	/**
	 * \brief Set weapon name and publish UI update.
	 * \param NewWeaponName 
	 */
	void SetWeaponNameAndPublishChange(FName NewWeaponName);
	
	/**
	 * \brief Set the crosshair texture and publish UI update.
	 * \param bIsPlayerAiming Is the player aiming down sights.
	 */
	void SetActiveCrosshairAndPublishChange(bool bIsPlayerAiming);

	/**
	 * \brief Set the wave states and publish UI update.
	 * \param OldState Wave state that is being exited.
	 * \param NewState Wave state that is being entered.
	 */
	void SetWaveStateAndPublishChange(EWaveState OldState, EWaveState NewState);

	/**
	 * \brief Set the wave number and publish UI update.
	 * \param WaveNumber Wave number to be displayed in the UI.
	 */
	void SetWaveNumberAndPublishChange(uint8 WaveNumber);

	/**
	 * \brief Use the wave states to return the correct text for the notification
	 * \param OldState 
	 * \param NewState 
	 * \return 
	 */
	FString GetWaveNotificationText(EWaveState OldState, EWaveState NewState);

	/**
	 * \brief Pointer to the currently equipped weapon.
	 */
	ANexusWeapon* CurrentWeapon;
};