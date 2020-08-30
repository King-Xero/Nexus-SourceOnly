// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NexusGameModeBase.generated.h"

enum class EWaveState : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled, AActor*, KilledActor, AController*, InstigatingController, AActor*, DeathCauser);

UCLASS()
class NEXUS_API ANexusGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ANexusGameModeBase();
	
	virtual void StartPlay() override;

	/**
	 * \brief Event used to broadcast when an actor has been killed.
	 */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnActorKilled OnActorKilled;

protected:

	/**
	 * \brief Called when the game starts.
	 */
	virtual void BeginPlay() override;
	
	/**
	 * \brief Hook to spawn an enemy in blueprint.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnNewEnemy();

	/**
	 * \brief Start spawning enemies.
	 */
	void StartWave();
	
	/**
	 * \brief Stop spawning enemies.
	 */
	void EndWave();

	/**
	 * \brief Set timer for next StartWave.
	 */
	void PrepareForNextWave();

	/**
	 * \brief Check if there are any alive enemies in the wave.
	 */
	UFUNCTION(BlueprintCallable, Category = "Waves")
	void CheckEnemiesAlive();

	/**
	 * \brief Check if there are any alive players.
	 */
	UFUNCTION(BlueprintCallable, Category = "Waves")
	void CheckPlayersAlive();

	/**
	 * \brief End the game/match.
	 */
	void GameOver();

	/**
	 * \brief Restart players that have died.
	 */
	void RepsawnDeadPlayers();

	/**
	 * \brief The starting number of enemies used to calculate total enemies in a wave.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameMode")
	int BaseEnemyCount = 2;

	/**
	 * \brief Interval at which enemies are spawned.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameMode")
	float EnemySpawnRate = 1.0f;

	/**
	 * \brief The maximum number of enemies that can be on the map at once.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameMode")
	int MaxEnemiesOnMap = 8;
	
	/**
	 * \brief The duration of the delay before the next wave starts.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameMode")
	float WaveDelayTime = 10.0f;

	/**
	 * \brief The duration of the delay between completing a wave and preparing the next wave.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameMode")
	float WaveCompleteDelayTime = 5.0f;

	/**
	 * \brief Interval at which the check to see if any players are alive is run.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameMode")
	float PlayerCheckRate = 1.0f;

	/**
	 * \brief Value used to set time dilation when an enemy is killed.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameMode")
	float SlowMoTimeDilation = 0.5f;

	/**
	 * \brief The duration (in seconds) of the slow mo effect when an enemy is killed.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameMode")
	float SlowMoDuration = 0.5f;

private:

	/**
	 * \brief Callback for the timer used to spawn enemies.
	 */
	void EnemySpawnerElapsed();

	/**
	 * \brief Called when the OnActorKilled event is broadcast.
	 * \param KilledActor The actor that was killed.
	 * \param InstigatingController The controller that killed KilledActor.
	 * \param DeathCauser The actor that killed KilledActor.
	 */
	UFUNCTION()
	void ActorKilled(AActor* KilledActor, AController* InstigatingController, AActor* DeathCauser);
	
	/**
	 * \brief Set the new wave state in the game mode game state.
	 * \param NewWaveState The wave state that the match is entering.
	 * @note The game mode game state is used to replicate game mode data across networked clients.
	 */
	void SetWaveState(EWaveState NewWaveState);

	/**
	 * \brief End the slow motion effect applied when an enemy is killed.
	 */
	void EndSlowMotionEffect();
	
	/**
	 * \brief The current wave number.
	 */
	int WaveCount;
	
	/**
	 * \brief The number of enemies to spawn in the current wave.
	 */
	int EnemiesToSpawn;

	/**
	 * \brief The number of enemies currently spawned on the map.
	 */
	int CurrentlySpawnedEnemies;
	
	/**
	 * \brief Handle used to manage timer that spawns enemies.
	 */
	FTimerHandle TimerHandle_EnemySpawner;

	/**
	 * \brief Handle used to manage timer that starts the next wave.
	 */
	FTimerHandle TimerHandle_StartNextWave;
	
	/**
	 * \brief Handle used to manage timer that delays the preparation of the next wave after a wave is completed.
	 */
	FTimerHandle TimerHandle_WaveComplete;
	
	/**
	 * \brief Handle used to manage the timer that stop the slow motion effect.
	 */
	FTimerHandle TimerHandle_SlowMotion;
};