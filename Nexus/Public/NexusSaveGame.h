// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "NexusPlayerScoreStruct.h"
#include "NexusSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class NEXUS_API UNexusSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, Category=Basic)
	TArray<FNexusPlayerScoreStruct> HighScores;
};
