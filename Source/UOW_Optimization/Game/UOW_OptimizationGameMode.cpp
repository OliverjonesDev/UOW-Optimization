#include "Game/UOW_OptimizationGameMode.h"
#include "UObject/ConstructorHelpers.h"

AUOW_OptimizationGameMode::AUOW_OptimizationGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
