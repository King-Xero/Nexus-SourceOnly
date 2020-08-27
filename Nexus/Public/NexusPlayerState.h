// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NexusPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerScoreUpdatedSignature, ANexusPlayerState*, PlayerState, float, NewScore);

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

	/**
	 * \brief Event used to broadcast ammo updates.
	 */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPlayerScoreUpdatedSignature OnScoreUpdated;
};