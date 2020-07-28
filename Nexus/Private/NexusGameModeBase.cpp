// Copyright Epic Games, Inc. All Rights Reserved.

#include "NexusGameModeBase.h"
#include "EngineUtils.h"
#include "Components/NexusHealthComponent.h"
#include "Nexus/Utils/Logging/NexusLogging.h"
#include "NexusGameState.h"
#include "NexusPlayerState.h"

ANexusGameModeBase::ANexusGameModeBase()
{
	// Set game state class to replicate data to clients using created class.
	GameStateClass = ANexusGameState::StaticClass();

	// Set player state class to assign player data using created class.
	PlayerStateClass = ANexusPlayerState::StaticClass();
}

/**
 * \brief Start the game/match.
 * @note Transitions to calls BeginPlay on actors.
 */
void ANexusGameModeBase::StartPlay()
{
	Super::StartPlay();

	PrepareForNextWave();
}

void ANexusGameModeBase::BeginPlay()
{
	OnActorKilled.AddDynamic(this, &ANexusGameModeBase::ActorKilled);
}

void ANexusGameModeBase::StartWave()
{
	++WaveCount;
	
	// Number of enemies spawned increases with waves progressed.
	EnemiesToSpawn = BaseEnemyCount * WaveCount;

	// Start timer that repeatedly spawns enemies.
	GetWorldTimerManager().SetTimer(TimerHandle_EnemySpawner, this, &ANexusGameModeBase::EnemySpawnerElapsed, EnemySpawnRate, true, 0.0f);

	// New wave is now in progress.
	SetWaveState(EWaveState::WaveInProgress);
}

void ANexusGameModeBase::EndWave()
{
	// Stop timer that spawns enemies.
	GetWorldTimerManager().ClearTimer(TimerHandle_EnemySpawner);

	// Waiting for players to kill all enemies.
	SetWaveState(EWaveState::WaitingToComplete);
}

void ANexusGameModeBase::PrepareForNextWave()
{
	GetWorldTimerManager().SetTimer(TimerHandle_StartNextWave, this, &ANexusGameModeBase::StartWave, WaveDelayTime);

	// Respawn any players that dies during the previous wave.
	RepsawnDeadPlayers();
	
	// Waiting for the next wave to start.
	SetWaveState(EWaveState::PreparingNextWave);
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
			// All enemies have been defeated, the wave is complete.
			SetWaveState(EWaveState::WaveComplete);
			
			// If there are no alive enemies, we should prepare the next wave.
			GetWorldTimerManager().SetTimer(TimerHandle_WaveComplete, this, &ANexusGameModeBase::PrepareForNextWave, WaveCompleteDelayTime);
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
	EndWave();

	// ToDo finish the match, and present "game over" screen to players.

	// The match has ended.
	SetWaveState(EWaveState::GameOver);

	ANexusGameState* CurrentGameState = GetGameState<ANexusGameState>();
	if (CurrentGameState)
	{
		// Call game over on game state for server and all clients.
		CurrentGameState->MulticastOnGameOver();
	}

	FNexusLogging::Log(ELogLevel::INFO, TEXT("All players dead. Game Over!!!"));
}

void ANexusGameModeBase::RepsawnDeadPlayers()
{
	for (TActorIterator<APlayerController> PlayerIterator(GetWorld()); PlayerIterator; ++PlayerIterator)
	{
		// Dereference the player controller from the iterator.
		APlayerController* PlayerToCheck = *PlayerIterator;
		// Check if the player is valid.
		if (PlayerToCheck && !PlayerToCheck->GetPawn())
		{
			// Spawn the player.
			RestartPlayer(PlayerToCheck);
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

void ANexusGameModeBase::ActorKilled(AActor* KilledActor, AController* InstigatingController, AActor* DeathCauser)
{
	// When an actor is killed, check if players and enemies are alive.
	CheckPlayersAlive();
	CheckEnemiesAlive();
	
	APawn* KilledPawn = Cast<APawn>(KilledActor);

	
	if (KilledPawn)
	{
		if (KilledPawn->IsPlayerControlled())
		{
			// A player was killed, so we should check if there are any still alive.
			CheckPlayersAlive();
			// Score is not awarded for killing other players.
		}
		else
		{
			// An enemy was killed, so we should check if there are any still alive.
			CheckEnemiesAlive();
			
			if (InstigatingController)
			{
				APawn* InstigatingPawn = InstigatingController->GetPawn();
				if (InstigatingPawn)
				{
					ANexusPlayerState* InstigatingPlayerState = InstigatingPawn->GetPlayerStateChecked<ANexusPlayerState>();
					if (InstigatingPlayerState)
					{
						// Award instigating player points.
						InstigatingPlayerState->AddScore(10);

						FStringFormatOrderedArguments LogArgs;
						LogArgs.Add(FStringFormatArg(InstigatingPawn->GetName()));
						LogArgs.Add(FStringFormatArg(KilledPawn->GetName()));
						LogArgs.Add(FStringFormatArg(10));
						LogArgs.Add(FStringFormatArg(InstigatingPlayerState->GetScore()));
						FNexusLogging::Log(ELogLevel::TRACE, FString::Format(TEXT("{0} killed {1}. {2} points awarded. Total score: {3}."), LogArgs));

						// ToDo Add an interface to pawns that has the amounts of points that they are worth.
					}
				}
			}
		}		
	}
}

void ANexusGameModeBase::SetWaveState(EWaveState NewWaveState)
{
	ANexusGameState* MatchGameState = GetGameState<ANexusGameState>();

	if (ensureAlways(MatchGameState))
	{
		// Set the wave state in the game state, so that it is replicated to networked clients.
		MatchGameState->SetWaveState(NewWaveState);
	}
}
