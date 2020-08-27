// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedStruct.h"
#include "NexusPlayerScoreStruct.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct NEXUS_API FNexusPlayerScoreStruct
{
	GENERATED_BODY()

public:

	FNexusPlayerScoreStruct(FName NewName, float NewScore)
		: Name(NewName)
		, Score(NewScore)
	{
	}
	
	FNexusPlayerScoreStruct()
	: Score(0)
	{
	};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerScore")
	FName Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerScore")
	float Score;
};
