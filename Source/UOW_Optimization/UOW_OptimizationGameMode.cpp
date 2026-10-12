// Copyright Epic Games, Inc. All Rights Reserved.

#include "UOW_OptimizationGameMode.h"
#include "UOW_OptimizationCharacter.h"
#include "UObject/ConstructorHelpers.h"

AUOW_OptimizationGameMode::AUOW_OptimizationGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
