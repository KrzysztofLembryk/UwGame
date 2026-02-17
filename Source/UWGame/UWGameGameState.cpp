#include "UWGameGameState.h"

AUWGameGameState::AUWGameGameState()
{
}

float AUWGameGameState::GetTimeLeftSeconds() const
{
	return TimeLeftSeconds;
}

float AUWGameGameState::GetCurrentScore() const
{
	return CurrentScore;
}
