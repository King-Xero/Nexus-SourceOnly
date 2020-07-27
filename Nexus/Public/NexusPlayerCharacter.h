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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNexusHUDUserWidget* CurrentHUDWidget;
	
	/**
	 * \brief HUD to spawn for the player.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
	TSubclassOf<UNexusHUDUserWidget> HUDWidgetClass;
};
