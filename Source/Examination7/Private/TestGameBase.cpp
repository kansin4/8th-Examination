#include "TestGameBase.h"
#include "TestInstance.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "TestPlayerController.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

ATestGameBase::ATestGameBase()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	LevelDuration = 30.0f;
	CurrentLevelIndex = 0;
	MaxLevels = 3;
	CurrentWaveIndex = 0;
	MaxWaves = 3;
	WaveDuration = 10.0f;
}

void ATestGameBase::BeginPlay()
{
	Super::BeginPlay();

	StartLevel();

	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,
		&ATestGameBase::UpdateHUD,
		0.1f,
		true
	);
}

int32 ATestGameBase::GetScore() const
{
	return Score;
}

void ATestGameBase::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UTestInstance* TestInstance = Cast<UTestInstance>(GameInstance);
		if (TestInstance)
		{
			// 현재 점수에 Amount를 더합니다.
			Score += Amount;
			// TestInstance에 점수를 추가합니다.
			TestInstance->AddToScore(Amount);
			// 디버그 메시지를 출력합니다.
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,
					FString::Printf(TEXT("Score: %d"), Score));
			}
		}
	}
}

void ATestGameBase::OnGameOver()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ATestPlayerController* TestPlayerController = Cast<ATestPlayerController>(PlayerController))
		{
			TestPlayerController->SetPause(true);
			TestPlayerController->ShowMainMenu(true);
		}
	}
}

void ATestGameBase::StartLevel()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ATestPlayerController* TestPlayerController = Cast<ATestPlayerController>(PlayerController))
		{
			TestPlayerController->ShowGameHUD();
		}
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UTestInstance* TestInstance = Cast<UTestInstance>(GameInstance);
		if (TestInstance)
		{
			CurrentLevelIndex = TestInstance->CurrentLevelIndex;
		}
	}

	CurrentWaveIndex = 0;
	StartWave();
}

void ATestGameBase::StartWave()
{
	UE_LOG(LogTemp, Warning, TEXT("Wave %d Started!"), CurrentWaveIndex + 1);

	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	int32 ItemToSpawn = 20 + (CurrentWaveIndex * 10); // 웨이브가 증가할수록 스폰되는 아이템의 개수를 증가시킴

	for (int32 i = 0; i < ItemToSpawn; i++)
	{
		if (FoundVolumes.Num() > 0)
		{
			ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
			if (SpawnVolume)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
				{
					SpawnedCoinCount++;
				}
			}
		}
	}

	GetWorldTimerManager().SetTimer(
		WaveTimerHandle,
		this,
		&ATestGameBase::OnWaveTimeUp,
		WaveDuration,
		false
	);
}

void ATestGameBase::OnWaveTimeUp()
{
	CurrentWaveIndex++;

	if (CurrentWaveIndex >= MaxWaves)
	{
		EndLevel();
	}
	else
	{
		StartWave();
	}
}

void ATestGameBase::OnCoinCollected()
{
	CollectedCoinCount++;
	UE_LOG(LogTemp, Warning, TEXT("Collected Coin: %d / %d "),
		CollectedCoinCount,
		SpawnedCoinCount);

	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
	{
		OnWaveTimeUp();
	}
}

void ATestGameBase::EndLevel()
{
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);
	GetWorldTimerManager().ClearTimer(HUDUpdateTimerHandle);
	CurrentLevelIndex++;

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UTestInstance* TestInstance = Cast<UTestInstance>(GameInstance);
		if (TestInstance)
		{
			TestInstance->CurrentLevelIndex = CurrentLevelIndex;
		}
	}

	if (CurrentLevelIndex >= MaxLevels)
	{
		OnGameOver();
		return;
	}

	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		UpdateHUD();
		OnGameOver();
	}
}

void ATestGameBase::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ATestPlayerController* TestPlayerController = Cast<ATestPlayerController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = TestPlayerController->GetHUDWidget())
			{
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
					FString TimeString = FString::Printf(TEXT("Time: %.1f"), RemainingTime);
					TimeText->SetText(FText::FromString(TimeString));
				}
				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
				{
					FString ScoreString = FString::Printf(TEXT("Score: %d"), Score);
					ScoreText->SetText(FText::FromString(ScoreString));
				}
				if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
				{
					FString LevelString = FString::Printf(TEXT("Level: %d"), CurrentLevelIndex + 1);
					LevelIndexText->SetText(FText::FromString(LevelString));
				}
			}
		}
	}
}