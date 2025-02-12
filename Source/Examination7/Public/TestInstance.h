
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TestInstance.generated.h"

/**
 * 
 */
UCLASS()
class EXAMINATION7_API UTestInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UTestInstance();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameData")
	int32 TotalScore;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameData")
	int32 CurrentLevelIndex;
	
	UFUNCTION(BlueprintCallable, Category = "GameData")
	void AddToScore(int32 Amount);
};
