#include "UWGameGameState.h"

AUWGameGameState::AUWGameGameState()
{
}

float AUWGameGameState::GetTimeLeftSeconds() const
{
	return TimeLeftSeconds;
}

void AUWGameGameState::AddMoreTimeSeconds(float TimeToAdd)
{
	if (TimeToAdd < 0.f)
	{
		return;
	}

	TimeLeftSeconds += TimeToAdd;
}

float AUWGameGameState::GetCurrentScore() const
{
	return CurrentScore;
}
