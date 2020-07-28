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

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void HealthChanged(UNexusHealthComponent* HealthComponent, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser) override;

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
};
