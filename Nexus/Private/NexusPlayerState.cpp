// Toyan Green © 2020

#include "NexusPlayerState.h"

void ANexusPlayerState::AddScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);

	OnScoreUpdated.Broadcast(this, GetScore());
}