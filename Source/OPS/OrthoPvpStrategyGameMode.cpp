// Copyright Epic Games, Inc. All Rights Reserved.

#include "OrthoPvpStrategyGameMode.h"
#include "OrthoPvpStrategyCharacter.h"
#include "UObject/ConstructorHelpers.h"

AOrthoPvpStrategyGameMode::AOrthoPvpStrategyGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
