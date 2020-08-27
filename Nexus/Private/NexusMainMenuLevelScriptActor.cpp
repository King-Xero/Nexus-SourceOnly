// Toyan Green © 2020


#include "NexusMainMenuLevelScriptActor.h"
#include "NexusSaveGame.h"
#include "Kismet/GameplayStatics.h"

void ANexusMainMenuLevelScriptActor::BeginPlay()
{
	UNexusSaveGame* ExistingSaveGame = Cast<UNexusSaveGame>(UGameplayStatics::LoadGameFromSlot(GameSaveSlotName, 0));

	// If existing save file is not found, we should create one and populate with some "fake" high scores.
	if (!ExistingSaveGame)
	{
		UNexusSaveGame* NewSaveGame = Cast<UNexusSaveGame>(UGameplayStatics::CreateSaveGameObject(UNexusSaveGame::StaticClass()));
		for (int i = 0; i < 10; ++i)
		{
			//ToDo pre-populate game save with more interesting score entries.
			FNexusPlayerScoreStruct HighScore("Bob", 100);
			
			NewSaveGame->HighScores.Add(HighScore);
		}

		UGameplayStatics::SaveGameToSlot(NewSaveGame, GameSaveSlotName, 0);
	}

	// Super is called here because save game creation code should be called before anything else.
	Super::BeginPlay();
}
