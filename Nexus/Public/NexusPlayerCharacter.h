// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "NexusCharacter.h"
#include "NexusPlayerCharacter.generated.h"

class UNexusHUDUserWidget;

UCLASS()
class NEXUS_API ANexusPlayerCharacter : public ANexusCharacter
{
	GENERATED_BODY()

public:

	/**
	 * \brief Make the character swap the currently equipped weapon.
	 */
	virtual void SwapWeapon() override;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void HealthChanged(UNexusHealthComponent* HealthComponent, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser) override;

	/**
	 * \brief Camera shake triggered when player is hit.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
	TSubclassOf<UCameraShake> FlinchCameraShake;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNexusHUDUserWidget* CurrentHUDWidget;
	
	/**
	 * \brief HUD to spawn for the player.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
	TSubclassOf<UNexusHUDUserWidget> HUDWidgetClass;

	/**
	 * \brief The class type used to find the game over view target actor.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
	TSubclassOf<AActor> GameOverViewTargetClass;

	/**
	 * \brief The duration over which the game over camera view is blended.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
	float GameOverViewBlendTime;

private:

	/**
	 * \brief Called after the camera fades in to enable player input and add the hud to the screen.
	 */
	void DelayedPlayerStart();

	/**
	 * \brief Play the camera shake effect for when the player is hit.
	 */
	void PlayFlinchCameraShake();
	
	/**
	 * \brief Handle used to manage the timer that delays player input and the HUD.
	 */
	FTimerHandle TimerHandle_PlayerStart;
};
