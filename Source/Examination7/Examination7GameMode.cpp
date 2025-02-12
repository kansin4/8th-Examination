// Copyright Epic Games, Inc. All Rights Reserved.

#include "Examination7GameMode.h"
#include "Examination7Character.h"
#include "UObject/ConstructorHelpers.h"

AExamination7GameMode::AExamination7GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
