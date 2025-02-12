#include "MineItem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"

AMineItem::AMineItem()
{
	PrimaryActorTick.bCanEverTick = true;

	ExplosionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollision"));
	ExplosionCollision->SetupAttachment(RootComponent);

	ExplosionDelay = 2.0f;
	ExplosionRadius = 300.0f;
	ExplosionDamage = 50;
	bHasExploded = false;
}

void AMineItem::ActivateItem(AActor* Activator)
{
	if (!bHasExploded)
	{
		bHasExploded = true;
		GetWorldTimerManager().SetTimer(ExplosionTimerHandle, this, &AMineItem::Explode, ExplosionDelay, false);
	}
}

void AMineItem::Explode()
{
	// 폭발 효과 생성
	if (ExplosionParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticle, GetActorLocation());
	}

	// 폭발 소리 재생
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}

	// 폭발 범위 내의 액터들에게 데미지 적용
	TArray<AActor*> IgnoredActors;
	UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);

	// 마인 아이템 제거
	Destroy();
}