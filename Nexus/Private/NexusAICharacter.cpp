// Toyan Green © 2020


#include "NexusAICharacter.h"
#include "Perception/AIPerceptionComponent.h"

ANexusAICharacter::ANexusAICharacter()
{
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
}
