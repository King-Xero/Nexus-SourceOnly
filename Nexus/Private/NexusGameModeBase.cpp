// Copyright Epic Games, Inc. All Rights Reserved.

#include "NexusGameModeBase.h"
#include "EngineUtils.h"
#include "Components/NexusHealthComponent.h"
#include "Nexus/Utils/Logging/NexusLogging.h"

/**
 * \brief Start the game/match.
 * @note Transitions to calls BeginPlay on actors.
 */
void ANexusGameModeBase::StartPlay()
{
	Super::StartPlay();

	// Start looping timer to check if there are any players alive.
	GetWorldTimerManager().SetTimer(TimerHandle_AlivePlayers, this, &ANexusGameModeBase::CheckPlayersAlive, PlayerCheckRate, true);

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
	// Stop timer that spawns enemies.
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

void ANexusGameModeBase::CheckPlayersAlive()
{
	bool bPlayerAlive = false;

	for (TActorIterator<APlayerController> PlayerIterator(GetWorld()); PlayerIterator; ++PlayerIterator)
	{
		// Dereference the player controller from the iterator.
		APlayerController* PlayerToCheck = *PlayerIterator;
		// Check if the player is valid.
		if (PlayerToCheck && PlayerToCheck->GetPawn())
		{
			APawn* PawnToCheck = PlayerToCheck->GetPawn();			
			UNexusHealthComponent* PawnHealthComponent = Cast<UNexusHealthComponent>(PawnToCheck->GetComponentByClass(UNexusHealthComponent::StaticClass()));
			// Player pawn should always have a health component, so we should validate.
			if (ensure(PawnHealthComponent) && 0.0f < PawnHealthComponent->GetCurrentHealth())
			{
				// Found an alive enemy, we should exit the loop immediately.
				bPlayerAlive = true;
				break;
			}
		}
	}

	if (!bPlayerAlive)
	{
		// If there are no alive enemies, we should prepare the next wave.
		GameOver();
	}
}

void ANexusGameModeBase::GameOver()
{
	// Stop looping timer to check if there are any players alive.
	GetWorldTimerManager().ClearTimer(TimerHandle_AlivePlayers);
	
	EndWave();

	// ToDo finish the match, and present "game over" screen to players.

	FNexusLogging::Log(ELogLevel::INFO, TEXT("All players dead. Game Over!!!"));
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
