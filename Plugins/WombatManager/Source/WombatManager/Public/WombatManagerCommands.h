// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "WombatManagerStyle.h"

class FWombatManagerCommands : public TCommands<FWombatManagerCommands>
{
public:

	FWombatManagerCommands()
		: TCommands<FWombatManagerCommands>(TEXT("WombatManager"), NSLOCTEXT("Contexts", "WombatManager", "WombatManager Plugin"), NAME_None, FWombatManagerStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};