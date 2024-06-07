// Copyright 2023 Jeonghyeon Ha. All Rights Reserved.

#include "GameplayTagAccessorCommands.h"

#define LOCTEXT_NAMESPACE "FGameplayTagAccessorModule"

void FGameplayTagAccessorCommands::RegisterCommands()
{
	UI_COMMAND(GameplayTagAccessorCreate, "GameplayTagAccessor", "Generate GameplayTag Accessors", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
