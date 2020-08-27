// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "NexusMainMenuLevelScriptActor.generated.h"

/**
 * 
 */
UCLASS()
class NEXUS_API ANexusMainMenuLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;

private:

	const FString GameSaveSlotName = "NexusGameSave";
};
