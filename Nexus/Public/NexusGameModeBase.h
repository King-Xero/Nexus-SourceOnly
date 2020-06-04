// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NexusGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class NEXUS_API ANexusGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void StartPlay() override;

protected:

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
	 * \brief The duration of the delay before the next wave starts.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameMode")
	float WaveDelayTime = 10.0f;

private:

	/**
	 * \brief Callback for the timer used to spawn enemies.
	 */
	void EnemySpawnerElapsed();
	
	/**
	 * \brief The current wave number.
	 */
	int WaveCount;
	
	/**
	 * \brief The number of enemies to spawn in the current wave.
	 */
	int EnemiesToSpawn;
	
	/**
	 * \brief Handle used to manage timer that spawns enemies.
	 */
	FTimerHandle TimerHandle_EnemySpawner;

	/**
	 * \brief Handle used to manage timer that starts the next wave.
	 */
	FTimerHandle TimerHandle_StartNextWave;
};