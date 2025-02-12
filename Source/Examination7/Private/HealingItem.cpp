#include "HealingItem.h"
#include "TestCharacter.h"

AHealingItem::AHealingItem()
{
	HealAmount = 20.0f;
	ItemType = "Healing";
}

void AHealingItem::ActivateItem(AActor* Activator)
{

	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		ATestCharacter* PlayerCharacter = Cast<ATestCharacter>(Activator);
		{
			PlayerCharacter->AddHealth(HealAmount);
		}
		DestroyItem();
	}
}