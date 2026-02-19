#include "UWGameGameMode.h"
#include "UWGameGameState.h"
#include "UWGameLog.h"
#include "UWGameInstance.h"

AUWGameGameMode::AUWGameGameMode()
{
	GameStateClass = AUWGameGameState::StaticClass();
	
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AUWGameGameMode::InitGameState()
{
	Super::InitGameState();

	UUWGameInstance* UWGameInstance = Cast<UUWGameInstance>(GetGameInstance());
	AUWGameGameState* UWGameState = Cast<AUWGameGameState>(GameState);
	
	if (UWGameInstance && UWGameState)
	{
		FLevelRulesData CurrentLevelRules = UWGameInstance->GetCurrentLevelRules();
		UWGameState->TimeLeftSeconds = CurrentLevelRules.LevelTimeSeconds;
		UWGameState->bTimeOut = false;
	}
}

void AUWGameGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (AUWGameGameState* UWGameState = Cast<AUWGameGameState>(GameState))
	{
		// When we are in 0 level, we have no time limit, so we do not want to tick down 
		// time or cause timeout
		if (UUWGameInstance* UWGameInstance = Cast<UUWGameInstance>(GetGameInstance()))
		{
			if (UWGameInstance->GetCurrentLevelNum() == 0)
			{
				return;
			}
		}

		if (UWGameState->TimeLeftSeconds <= 0)
		{
			if (UWGameState->bTimeOut == false)
			{
				if (UUWGameInstance* UWGameInstance = Cast<UUWGameInstance>(GetGameInstance()))
				{
					UWGameInstance->TimeOut();
				}
			}
			UWGameState->bTimeOut = true;
			return;
		}

		UWGameState->TimeLeftSeconds -= DeltaSeconds;

		if (UUWGameInstance* UWGameInstance = Cast<UUWGameInstance>(GetGameInstance()))
		{
			UWGameState->CurrentScore = UWGameInstance->GetScoreSum();
		}
	}
}
