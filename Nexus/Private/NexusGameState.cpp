// Toyan Green © 2020

#include "NexusGameState.h"
#include "Net/UnrealNetwork.h"
#include "Blueprint/UserWidget.h"
#include "EngineUtils.h"
#include "NexusPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "NexusSaveGame.h"
#include "NexusBackgroundMusicPlayer.h"

void ANexusGameState::SetWaveState(EWaveState NewWaveState)
{
	if (CurrentWaveState != NewWaveState)
	{
		if (ROLE_Authority == GetLocalRole())
		{
			const EWaveState OldWaveState = CurrentWaveState;

			// Setting the wave state will replicate to network clients.
			CurrentWaveState = NewWaveState;

			if (EWaveState::PreparingNextWave == CurrentWaveState)
			{
				++CurrentWaveNumber;
				// Update the wave counter UI.
				OnWaveNumberUpdated.Broadcast(this, CurrentWaveNumber);
			}

			// Change the wave state locally on the server authority.
			OnRep_WaveState(OldWaveState);
		}
	}
}

int ANexusGameState::GetCurrentWaveNumber() const
{
	return CurrentWaveNumber;
}

bool ANexusGameState::IsNewHighScore()
{
	return bNewHighScore;
}

ANexusBackgroundMusicPlayer* ANexusGameState::GetMusicPlayer()
{
	return GameMusicPlayer;
}

void ANexusGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Ensure that CurrentWaveState is replicated, so the clients can use it.
	DOREPLIFETIME(ANexusGameState, CurrentWaveState);

	// Ensure that CurrentWaveNumber is replicated, so the clients can use it.
	DOREPLIFETIME(ANexusGameState, CurrentWaveNumber);
}

void ANexusGameState::BeginPlay()
{
	Super::BeginPlay();

	CurrentWaveNumber = 0;

	bGameOver = false;

	bNewHighScore = false;

	// Spawn music player
	if (GameMusicPlayerClass)
	{
		GameMusicPlayer = Cast<ANexusBackgroundMusicPlayer>(GetWorld()->SpawnActor(GameMusicPlayerClass));
	}
}

void ANexusGameState::OnRep_WaveState(EWaveState OldState)
{
	WaveStateChanged(OldState, CurrentWaveState);

	SetMusicForWaveState(CurrentWaveState);

	OnWaveStateUpdated.Broadcast(this, OldState, CurrentWaveState);
}

void ANexusGameState::OnRep_CurrentWaveNumber()
{
	OnWaveNumberUpdated.Broadcast(this, CurrentWaveNumber);
}

void ANexusGameState::SavePlayerScore()
{
	for (TActorIterator<APlayerController> PlayerIterator(GetWorld()); PlayerIterator; ++PlayerIterator)
	{
		// Dereference the player controller from the iterator.
		APlayerController* PlayerToCheck = *PlayerIterator;
		// Check if the player is valid.
		if (PlayerToCheck && PlayerToCheck->IsLocalPlayerController())
		{
			ANexusPlayerState* PlayerState = PlayerToCheck->GetPlayerState<ANexusPlayerState>();
			if (PlayerState)
			{
				const float FinalScore = PlayerState->GetScore();

				UNexusSaveGame* SaveGame = Cast<UNexusSaveGame>(UGameplayStatics::LoadGameFromSlot(GameSaveSlotName, 0));
				if (SaveGame)
				{
					FNexusPlayerScoreStruct ScoreToAdd(FName(FPlatformProcess::UserName()), FinalScore);

					// Loop through all scores in the game save, insert the new score, and shuffle down existing lower scores.
					for (int i = 0; i < SaveGame->HighScores.Num(); ++i)
					{
						if(SaveGame->HighScores[i].Score < FinalScore)
						{
							// Set flag so that the new high score UI and SFX are activated.
							bNewHighScore = true;
							
							const FNexusPlayerScoreStruct TempScore = SaveGame->HighScores[i];

							SaveGame->HighScores[i] = ScoreToAdd;

							ScoreToAdd = TempScore;
						}
					}

					UGameplayStatics::SaveGameToSlot(SaveGame, GameSaveSlotName, 0);
				}
			}
		}
	}
}

void ANexusGameState::SetMusicForWaveState(EWaveState WaveState) const
{
	if (GameMusicPlayer)
	{
		switch (WaveState)
		{
		case EWaveState::WaveInProgress:
			GameMusicPlayer->PlayRandomTrackFromList();
			break;	
		case EWaveState::WaveComplete:
			GameMusicPlayer->StopMusic();
			break;
		case EWaveState::GameOver:
			GameMusicPlayer->StopMusic();
			break;
		case EWaveState::PreparingNextWave:
		case EWaveState::WaitingToComplete:
		case EWaveState::Unknown:
		default:
			break;
		}
	}
}

void ANexusGameState::MulticastOnGameOver_Implementation()
{
	if (!bGameOver)
	{
		// Save player score.
		SavePlayerScore();

		// Create and show the game over widget.
		UUserWidget* GameOverWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);

		GameOverWidget->AddToViewport();

		bGameOver = true;
	}	
}
