// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NexusGameState.generated.h"

/**
 * \brief Used to declare the output method for log messages.
 */
UENUM(BlueprintType)
enum class EWaveState : uint8
{
	/**
	 * \brief Invalid wave state.
	 */
	Unknown				UMETA(DisplayName = "Unknown"),
	/**
	 * \brief Waiting to start next wave.
	 */
	PreparingNextWave	UMETA(DisplayName = "Preparing next wave"),
	/**
	 * \brief Wave in progress, with enemies being spawned.
	 */
	WaveInProgress		UMETA(DisplayName = "Wave in progrss"),
	/**
	 * \brief Wave in progress, waiting for players to kill remaining enemies.
	 */
	WaitingToComplete	UMETA(DisplayName = "Waiting to complete"),
	/**
	 * \brief Wave has been completed.
	 */
	WaveComplete		UMETA(DisplayName = "Wave complete"),
	/**
	 * \brief The match/game has ended.
	 */
	GameOver			UMETA(DisplayName = "Game over"),
};

/**
 * 
 */
UCLASS()
class NEXUS_API ANexusGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	/**
	 * \brief Set the current wave state.
	 * \param NewWaveState The wave state that the match is entering.
	 */
	void SetWaveState(EWaveState NewWaveState);
	
	UPROPERTY(ReplicatedUsing=OnRep_WaveState, BlueprintReadOnly, Category = "GameState")
	EWaveState CurrentWaveState;

protected:
	
	/**
	 * \brief Replicate the game/match wave state.
	 * \param OldState The wave state that the match is leaving.
	 */
	UFUNCTION()
	void OnRep_WaveState(EWaveState OldState);

	/**
	 * \brief Hook to implement wave state change in blueprint.
	 * \param OldState The wave state that the match is leaving.
	 * \param NewState The wave state that the match is entering.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "GameState")
	void WaveStateChanged(EWaveState OldState, EWaveState NewState);
};