#include "SpawnVolume.h"
#include "Components/BoxComponent.h"

ASpawnVolume::ASpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject <USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	SpawningBox->SetupAttachment(Scene);

	ItemDataTable = nullptr;
}

FVector ASpawnVolume::GetRandomPointInVolume() const
{
	FVector BoxExtent = SpawningBox->GetScaledBoxExtent(); // GetScaledBoxExtent() : 박스의 크기를 반환
	// (200, 100, 50) (2, 1, 1) -> (400, 100, 50) 
	// 중심 -> 끝까지의 거리를 반환해줌
	FVector BoxOrigin = SpawningBox->GetComponentLocation(); // 박스의 중심 위치를 반환

	return BoxOrigin + FVector(
		FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
		FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
		FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z)
	);
}

AActor* ASpawnVolume::SpawnRandomItem()
{
	if (FItemSpawnRow* SelectedRow = GetRandomItem())
	{
		if (UClass* ActualClass = SelectedRow->ItemClass.Get())
		{
			return SpawnItem(ActualClass);
		}
	}

	return nullptr;
}

FItemSpawnRow* ASpawnVolume::GetRandomItem() const
{
	if (!ItemDataTable) return nullptr;

	TArray<FItemSpawnRow*> AllRows;
	static const FString ContextString(TEXT("ItemSpawnContext")); // 디버깅용 문자열
	ItemDataTable->GetAllRows(ContextString, AllRows);

	if (AllRows.IsEmpty()) return nullptr; // 데이터가 없으면 nullptr 반환

	float TotalChance = 0.f;
	for (const FItemSpawnRow* Row : AllRows)
	{
		if (Row)
		{
			TotalChance += Row->SpawnChance;
		}
	}

	const float RandValue = FMath::FRandRange(0.f, TotalChance); // 0 ~ TotalChance 사이의 랜덤값을 반환
	float AccumulateChance = 0.f;

	for (FItemSpawnRow* Row : AllRows)
	{
		AccumulateChance += Row->SpawnChance;
		if (RandValue <= AccumulateChance)
		{
			return Row;
		}
	}

	return nullptr;

	// 0.0 ~ 1.0
	// 랜덤값 0.7이 나왔다?
	// Item A 0.5
	// Item B 0.8
	// Item C 1.0

	// 빨간공이 5개 (50%)
	// 파란공이 3개 (30%)
	// 초록공이 2개 (20%)
	// 전체 공 10개
	// 0~1 사이의 랜덤값을 뽑아서
	// 0.3이 나왔다. -> 빨간공? 
	// 확률 분포의 누적 확률 방식, 복원 추출 방식, 비복원 추출 방식(뽑은 것을 다시 넣지 않음)


}
AActor* ASpawnVolume::SpawnItem(TSubclassOf<AActor> ItemClass)
{
	if (!ItemClass) return nullptr;

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
		ItemClass,
		GetRandomPointInVolume(),
		FRotator::ZeroRotator
	);

	return SpawnedActor;
}


