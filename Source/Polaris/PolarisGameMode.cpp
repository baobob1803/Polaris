// Copyright Epic Games, Inc. All Rights Reserved.

#include "PolarisGameMode.h"
#include "PolarisCharacter.h"
#include "UObject/ConstructorHelpers.h"

APolarisGameMode::APolarisGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
