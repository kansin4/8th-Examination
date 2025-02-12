#pragma once

#include "CoreMinimal.h"
#include "CoinItem.h"
#include "BigCoinItem.generated.h"

UCLASS()
class EXAMINATION7_API ABigCoinItem : public ACoinItem
{
	GENERATED_BODY()
	
public:
	ABigCoinItem(); // 생성자

	virtual void ActivateItem(AActor* Activator) override; // 아이템 활성화 함수


};
