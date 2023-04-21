// Copyright 2023 Jeonghyeon Ha. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "GameplayTagAccessorStyle.h"

class FGameplayTagAccessorCommands : public TCommands<FGameplayTagAccessorCommands>
{
public:

	FGameplayTagAccessorCommands()
		: TCommands<FGameplayTagAccessorCommands>(TEXT("GameplayTagAccessor"), NSLOCTEXT("Contexts", "GameplayTagAccessor", "GameplayTagAccessor Plugin"), NAME_None, FGameplayTagAccessorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
