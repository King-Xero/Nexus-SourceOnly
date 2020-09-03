// Toyan Green © 2020


#include "NexusMainMenuLevelScriptActor.h"
#include "NexusSaveGame.h"
#include "Kismet/GameplayStatics.h"

void ANexusMainMenuLevelScriptActor::BeginPlay()
{
	UNexusSaveGame* ExistingSaveGame = Cast<UNexusSaveGame>(UGameplayStatics::LoadGameFromSlot(GameSaveSlotName, 0));

	// If existing save file is not found, we should create one and populate it with a list of high scores.
	if (!ExistingSaveGame)
	{
		UNexusSaveGame* NewSaveGame = Cast<UNexusSaveGame>(UGameplayStatics::CreateSaveGameObject(UNexusSaveGame::StaticClass()));
		NewSaveGame->HighScores.Add(FNexusPlayerScoreStruct("Toyan Green", 25000));
		NewSaveGame->HighScores.Add(FNexusPlayerScoreStruct("Ben Richards", 24000));
		NewSaveGame->HighScores.Add(FNexusPlayerScoreStruct("Dutch", 23000));
		NewSaveGame->HighScores.Add(FNexusPlayerScoreStruct("John Matrix", 22000));
		NewSaveGame->HighScores.Add(FNexusPlayerScoreStruct("T-800", 21000));
		NewSaveGame->HighScores.Add(FNexusPlayerScoreStruct("Jack Slater", 20000));
		NewSaveGame->HighScores.Add(FNexusPlayerScoreStruct("Harry Tasker", 19000));
		NewSaveGame->HighScores.Add(FNexusPlayerScoreStruct("John Kimble", 18000));		
		NewSaveGame->HighScores.Add(FNexusPlayerScoreStruct("Douglas Quaid", 17000));		
		NewSaveGame->HighScores.Add(FNexusPlayerScoreStruct("Victor Fries", 16000));

		UGameplayStatics::SaveGameToSlot(NewSaveGame, GameSaveSlotName, 0);
	}

	// Super is called here because save game creation code should be called before anything else.
	Super::BeginPlay();
}
