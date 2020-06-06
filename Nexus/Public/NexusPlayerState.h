// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NexusPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class NEXUS_API ANexusPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	/**
	 * \brief Amend the total score amount.
	 * \param ScoreAmount Amount of score to amend.
	 */
	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void AddScore(float ScoreAmount);
};