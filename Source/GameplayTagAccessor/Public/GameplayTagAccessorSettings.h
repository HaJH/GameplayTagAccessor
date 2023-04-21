// Copyright 2023 Jeonghyeon Ha. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagAccessorSettings.generated.h"

/**
 * 
 */
UCLASS(config=Game, defaultconfig)
class GAMEPLAYTAGACCESSOR_API UGameplayTagAccessorSettings : public UObject
{
	GENERATED_BODY()
public:
	static FORCEINLINE UGameplayTagAccessorSettings* Get()
	{
		UGameplayTagAccessorSettings* Settings = GetMutableDefault<UGameplayTagAccessorSettings>();
		check(Settings);
		return Settings;
	}
	
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "GameplayTagAccessor|File")
	FString DirectoryPath = "Source/";
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "GameplayTagAccessor|File")
	FString FileName = "TagAccessors";
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "GameplayTagAccessor|File")
	bool bGenerateCppFile = true;
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "GameplayTagAccessor|File")
	FString NamespaceName = "Tags";

	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "GameplayTagAccessor|Source", meta = (ToolTip="Find all tags in the project and create corresponding variables."))
	bool bUseAllTagSources = true;
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "GameplayTagAccessor|Source", meta = (EditCondition = "!bUseAllTagSources", ToolTip="Find tags in the specified tag source and create corresponding variables."))
	TArray<FName> ExplicitTagSources = { "DefaultGameplayTags.ini", "YourExplicitTags.ini", "/Game/YourExplicitTagTable" };
	
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "GameplayTagAccessor|Accessor")
	FString TagPrefix = "Tag_";
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "GameplayTagAccessor|Accessor", meta=(ToolTip="Create an FName variable corresponding to the tag."))
	bool bUseNameAccessor = true;
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "GameplayTagAccessor|Accessor", meta = (EditCondition = "bUseNameAccessor"))
	FString NamePrefix = "Name_";
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "GameplayTagAccessor|Accessor", meta = (EditCondition = "bUseNameAccessor", ToolTip="Tags are collected together with tags, and names are collected together with names."))
	bool bAlternativeSortingForName = false;
	
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "GameplayTagAccessor|Automation", meta = (ToolTip="Automatically generate tag accessors when the project starts up."))
	bool bUseAutomaticGenerationWhenStartUp = false;
};
