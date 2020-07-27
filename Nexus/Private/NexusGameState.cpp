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

		if (EWaveState::PreparingNextWave == CurrentWaveState)
		{
			++CurrentWaveNumber;
		}

		// These events will update the wave UI elements on player screens.
		OnWaveStateUpdated.Broadcast(this, OldWaveState, CurrentWaveState);
		OnWaveNumberUpdated.Broadcast(this, CurrentWaveNumber);
	}	
}

uint8 ANexusGameState::GetCurrentWaveNumber() const
{
	return CurrentWaveNumber;
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
}

void ANexusGameState::OnRep_WaveState(EWaveState OldState)
{
	WaveStateChanged(OldState, CurrentWaveState);

	OnWaveStateUpdated.Broadcast(this, OldState, CurrentWaveState);
}

void ANexusGameState::OnRep_CurrentWaveNumber()
{
	OnWaveNumberUpdated.Broadcast(this, CurrentWaveNumber);
}
