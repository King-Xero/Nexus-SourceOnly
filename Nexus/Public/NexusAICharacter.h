// Toyan Green © 2020

#pragma once

#include "CoreMinimal.h"
#include "NexusCharacter.h"
#include "NexusAICharacter.generated.h"

class UAIPerceptionComponent;

UCLASS()
class NEXUS_API ANexusAICharacter : public ANexusCharacter
{
	GENERATED_BODY()
	
	ANexusAICharacter();

protected:

	/**
	 * \brief Component used to give AI senses. (Sight etc.)
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAIPerceptionComponent* PerceptionComponent;
};
