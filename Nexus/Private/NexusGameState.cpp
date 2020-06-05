// Toyan Green © 2020

#include "NexusGameState.h"
#include "Net/UnrealNetwork.h"

void ANexusGameState::SetWaveState(EWaveState NewWaveState)
{
	if (ROLE_Authority == GetLocalRole())
	{
		const EWaveState OldWaveState = CurrentWaveState;

		// Setting the wave state will replicate to network clients.
		CurrentWaveState = NewWaveState;

		// Change the wave state locally on the server authority.
		OnRep_WaveState(OldWaveState);
	}	
}

void ANexusGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Ensure that CurrentWaveState is replicated, so the clients can use it.
	DOREPLIFETIME(ANexusGameState, CurrentWaveState);
}

void ANexusGameState::OnRep_WaveState(EWaveState OldState)
{
	WaveStateChanged(OldState, CurrentWaveState);
}
