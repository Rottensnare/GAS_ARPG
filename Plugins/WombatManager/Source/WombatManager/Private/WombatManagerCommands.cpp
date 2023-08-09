// Copyright Epic Games, Inc. All Rights Reserved.

#include "WombatManagerCommands.h"

#define LOCTEXT_NAMESPACE "FWombatManagerModule"

void FWombatManagerCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "WombatManager", "Bring up WombatManager window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
