// Copyright Epic Games, Inc. All Rights Reserved.

#include "NexusGameModeBase.h"
#include "EngineUtils.h"
#include "Components/NexusHealthComponent.h"

/**
 * \brief Start the game/match.
 * @note Transitions to calls BeginPlay on actors.
 */
void ANexusGameModeBase::StartPlay()
{
	Super::StartPlay();

	PrepareForNextWave();
}

void ANexusGameModeBase::StartWave()
{
	++WaveCount;
	
	// Number of enemies spawned increases with waves progressed.
	EnemiesToSpawn = BaseEnemyCount * WaveCount;

	// Start timer that repeatedly spawns enemies.
	GetWorldTimerManager().SetTimer(TimerHandle_EnemySpawner, this, &ANexusGameModeBase::EnemySpawnerElapsed, EnemySpawnRate, true, 0.0f);
}

void ANexusGameModeBase::EndWave()
{
	// Stop timer that spawns enemies
	GetWorldTimerManager().ClearTimer(TimerHandle_EnemySpawner);
}

void ANexusGameModeBase::PrepareForNextWave()
{
	GetWorldTimerManager().SetTimer(TimerHandle_StartNextWave, this, &ANexusGameModeBase::StartWave, WaveDelayTime);
}

void ANexusGameModeBase::CheckEnemiesAlive()
{
	// We should only check for alive enemies, if there are no enemies left to spawn and next wave timer is not active.
	if (0 >= EnemiesToSpawn && !GetWorldTimerManager().IsTimerActive(TimerHandle_StartNextWave))
	{
		bool bEnemyAlive = false;

		for (TActorIterator<APawn> PawnIterator(GetWorld()); PawnIterator; ++PawnIterator)
		{
			// Dereference the pawn from the iterator.
			APawn* PawnToCheck = *PawnIterator;
			// Check if the pawn is valid, and is not a player.
			if (PawnToCheck && !PawnToCheck->IsPlayerControlled())
			{
				UNexusHealthComponent* PawnHealthComponent = Cast<UNexusHealthComponent>(PawnToCheck->GetComponentByClass(UNexusHealthComponent::StaticClass()));
				if (PawnHealthComponent && 0.0f < PawnHealthComponent->GetCurrentHealth())
				{
					// Found an alive enemy, we should exit the loop immediately.
					bEnemyAlive = true;
					break;
				}
			}
		}

		if (!bEnemyAlive)
		{
			// If there are no alive enemies, we should prepare the next wave.
			PrepareForNextWave();
		}
	}	
}

void ANexusGameModeBase::EnemySpawnerElapsed()
{
	// Spawn an enemy
	SpawnNewEnemy();

	if (0 >= --EnemiesToSpawn)
	{
		EndWave();
	}
}
