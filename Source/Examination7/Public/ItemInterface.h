// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemInterface.generated.h"

UINTERFACE(MinimalAPI)
class UItemInterface : public UInterface
{
	GENERATED_BODY()
};

class EXAMINATION7_API IItemInterface
{
	GENERATED_BODY()

public:
	UFUNCTION() // UFUNCTION()을 붙이면 블루프린트에서 사용 가능
	virtual void OnItemOverlap(
		UPrimitiveComponent* OverlappedComp, // 오버랩된 자기자신의 콜리전 여기선 스피어 컴포넌트
		AActor* OtherActor, // 오버랩된 액터, 여기선 플레이어
		UPrimitiveComponent* OtherComp, // 액터에 붙어있던 충돌을 일으킨 컴포넌트, 여기선 플레이어의 캡슐 컴포넌트
		int32 OtherBodyIndex, 
		bool bFromSweep,
		const FHitResult& SweepResult) = 0; // = 0; 을 붙이면 재정의에 강제성을 부여
	UFUNCTION()
	virtual void OnItemEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex) = 0; // 순수 가상 함수
	virtual void ActivateItem(AActor* Activator) = 0;
	virtual FName GetItemType() const = 0; // FString 보다 빨라서 사용

	// 지뢰, 힐링, 코인
	// 힐링, 코인 - 즉시 발동형 - 오버랩
	// 지뢰 - 범위 내에 오버랩 - 발동이 5초뒤 폭발 - 오버랩 - 데미지
};
