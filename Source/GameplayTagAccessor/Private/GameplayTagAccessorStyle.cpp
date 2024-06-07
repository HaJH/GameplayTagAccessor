// Copyright 2023 Jeonghyeon Ha. All Rights Reserved.

#include "GameplayTagAccessorStyle.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FGameplayTagAccessorStyle::StyleInstance = nullptr;

void FGameplayTagAccessorStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FGameplayTagAccessorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FGameplayTagAccessorStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("GameplayTagAccessorStyle"));
	return StyleSetName;
}


const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FGameplayTagAccessorStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("GameplayTagAccessorStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("GameplayTagAccessor")->GetBaseDir() / TEXT("Resources"));

	Style->Set("GameplayTagAccessor.GameplayTagAccessorCreate", new IMAGE_BRUSH(TEXT("icon128"), Icon20x20));
	return Style;
}

void FGameplayTagAccessorStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FGameplayTagAccessorStyle::Get()
{
	return *StyleInstance;
}
