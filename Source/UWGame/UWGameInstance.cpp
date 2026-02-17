#include "UWGameInstance.h"

#include "UWGameGameState.h"
#include "UWGameUtils.h"
#include "Kismet/GameplayStatics.h"

void UUWGameInstance::Init()
{
	Super::Init();

	LoadGameData();
}

float UUWGameInstance::GetScoreSum() const
{
	return ScoreSum;
}

float UUWGameInstance::GetTimeLeftInLevel() const
{
	if (AUWGameGameState* GameState = Cast<AUWGameGameState>(GetWorld()->GetGameState()))
	{
		return GameState->GetTimeLeftSeconds();
	}

	return 0.f;
}

void UUWGameInstance::StartNewGame()
{
	CurrentLevelNum = 1;
	ScoreSum = 0.f;

	OpenLevel(CurrentLevelNum);
}

void UUWGameInstance::SetPlayerName(FString InPlayerName)
{
	PlayerName = InPlayerName;
}

TArray<FScoreRow> UUWGameInstance::GetAllScores() const
{
	return SaveGameObject->Scores;
}

bool UUWGameInstance::OpenLevel(int32 LevelNum)
{
	if (GameSettingsTable)
	{
		TArray<FLevelRulesData*> ResultRows;
		const FString ContextString;
		GameSettingsTable->GetAllRows<FLevelRulesData>(ContextString, ResultRows);

		for (int32 i = 0; i < ResultRows.Num(); i++)
		{
			if (ResultRows[i]->LevelNumber == LevelNum)
			{
				CurrentLevelRules = *ResultRows[i];
				UGameplayStatics::OpenLevelBySoftObjectPtr(this, CurrentLevelRules.LevelPath, true);
				return true;
			}
		}
	}

	return false;
}

void UUWGameInstance::AddScore(float ScoreToAdd)
{
	ScoreSum += ScoreToAdd;
}

void UUWGameInstance::FinishGame()
{
	FScoreRow NewScore;
	NewScore.PlayerName = PlayerName;
	NewScore.PlayerScore = ScoreSum;

	SaveGameObject->Scores.Add(NewScore);
	SaveGameObject->Scores.Sort([](const FScoreRow& A, const FScoreRow& B)
	{
		return A.PlayerScore > B.PlayerScore;
	});
	
	SaveGameData();
	
	OpenHighScoreMenu();
}

void UUWGameInstance::TimeOut()
{
	if (GetScoreSum() > CurrentLevelRules.MinScoreToNextLevel)
	{
		CurrentLevelNum++;
		bool bLevelFound = OpenLevel(CurrentLevelNum);
		if (! bLevelFound)
		{
			FinishGame();
		}
	}
	else
	{
		FinishGame();
	}
	
}

FLevelRulesData UUWGameInstance::GetCurrentLevelRules() const
{
	return CurrentLevelRules;
}

void UUWGameInstance::LoadGameData()
{
	UUWSaveGame* TempSaveGame =  Cast<UUWSaveGame>(UGameplayStatics::LoadGameFromSlot("Slot1", 0));
	if (TempSaveGame != nullptr)
	{
		SaveGameObject = TempSaveGame;
	}
	else
	{
		SaveGameObject = Cast<UUWSaveGame>(UGameplayStatics::CreateSaveGameObject(UUWSaveGame::StaticClass()));
	}
}

void UUWGameInstance::SaveGameData()
{
	if (SaveGameObject)
	{
		UGameplayStatics::SaveGameToSlot(SaveGameObject, "Slot1", 0);
	}
}
