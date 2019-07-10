// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "TPS_studyGameMode.h"
#include "TPS_studyCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATPS_studyGameMode::ATPS_studyGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
