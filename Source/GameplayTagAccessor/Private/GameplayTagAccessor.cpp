// Copyright 2023 Jeonghyeon Ha. All Rights Reserved.

#include "GameplayTagAccessor.h"
#include "GameplayTagAccessorStyle.h"
#include "GameplayTagAccessorCommands.h"
#include "ISettingsModule.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"
#include "GameplayTagAccessorSettings.h"

#include <fstream>
#include <sstream>
#include <string>

#include "GameplayTagsManager.h"
#include "GameplayTagsSettings.h"

static const FName GameplayTagAccessorTabName("GameplayTagAccessor");

#define LOCTEXT_NAMESPACE "FGameplayTagAccessorModule"

void FGameplayTagAccessorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Register button
	FGameplayTagAccessorStyle::Initialize();
	FGameplayTagAccessorStyle::ReloadTextures();

	FGameplayTagAccessorCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FGameplayTagAccessorCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FGameplayTagAccessorModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FGameplayTagAccessorModule::RegisterMenus));


	// Register settings
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "GameplayTagAccessor",
		                                 INVTEXT("GameplayTagAccessor"),
		                                 INVTEXT("Configure the GameplayTagAccessor settings"),
		                                 GetMutableDefault<UGameplayTagAccessorSettings>()
		);
	}

	if(UGameplayTagAccessorSettings::Get()->bUseAutomaticGenerationWhenStartUp)
	{
		GenerateGameplayAccessors();
	}
}

void FGameplayTagAccessorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FGameplayTagAccessorStyle::Shutdown();

	FGameplayTagAccessorCommands::Unregister();

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "GameplayTagAccessor");
	}
}

void FGameplayTagAccessorModule::PluginButtonClicked()
{
	FString TagSources;
	if(UGameplayTagAccessorSettings::Get()->bUseAllTagSources)
	{
		TagSources = TEXT("All Tag Sources");
	}
	else
	{
		for(const auto& TagSource : UGameplayTagAccessorSettings::Get()->ExplicitTagSources)
		{
			TagSources += TagSource.ToString() + "\n";
		}
	}

	FString Path = UGameplayTagAccessorSettings::Get()->DirectoryPath;
	// Append Path Separator if not exist
	if (!Path.EndsWith(FString("/")))
	{
		Path += FString("/");
	}
	
	if (GenerateGameplayAccessors())
	{
		FText DialogText = FText::Format(
			LOCTEXT("PluginButtonDialogText", "GameplayTag Accessors successfully generated:\n\n{0}{1}.h\n\nfrom\n\n{2}"),
			FText::FromString(Path),
			FText::FromString(UGameplayTagAccessorSettings::Get()->FileName),
			FText::FromString(TagSources)
		);
		FMessageDialog::Open(EAppMsgType::Ok, DialogText);
	}
	else
	{
		FText DialogText = FText::Format(
			LOCTEXT("PluginButtonDialogText_Failed",
			        "Error\n\nGameplayTag Accessors can't be generated:\n\n{0}{1}.h\n\nfrom\n\n{2}\n\nPlease check your settings."),
			FText::FromString(Path),
			FText::FromString(UGameplayTagAccessorSettings::Get()->FileName),
			FText::FromString(TagSources)
		);
		FMessageDialog::Open(EAppMsgType::Ok, DialogText);
	}
}

bool FGameplayTagAccessorModule::GenerateGameplayAccessors()
{
	try
	{
		FString Namespace = UGameplayTagAccessorSettings::Get()->NamespaceName;
		FString Path = UGameplayTagAccessorSettings::Get()->DirectoryPath;
		// Append Project Directory and Path
		FString ProjectDir = FPaths::ProjectDir();
		Path = ProjectDir + Path;
		// Append Path Separator if not exist
		if (!Path.EndsWith(FString("/")))
		{
			Path += FString("/");
		}
		FString FullPath = Path + UGameplayTagAccessorSettings::Get()->FileName + ".h";

		// Head
		std::string namespace_name = TCHAR_TO_UTF8(*Namespace);
		std::ostringstream header_file_content;
		header_file_content << "#pragma once\n\n";
		header_file_content << "#include \"CoreMinimal.h\"\n";
		header_file_content << "#include \"GameplayTags.h\"\n";
		header_file_content << "\nnamespace " << namespace_name << "\n";
		header_file_content << "{\n";
		// ~Head

		// Body
		// For all tag source, get all tags
		const auto& ExplicitTagSources = UGameplayTagAccessorSettings::Get()->ExplicitTagSources;
		TArray<const FGameplayTagSource*> TagSources;
		UGameplayTagsManager::Get().FindTagSourcesWithType(EGameplayTagSourceType::Native, TagSources);
		UGameplayTagsManager::Get().FindTagSourcesWithType(EGameplayTagSourceType::DefaultTagList, TagSources);
		UGameplayTagsManager::Get().FindTagSourcesWithType(EGameplayTagSourceType::TagList, TagSources);
		UGameplayTagsManager::Get().FindTagSourcesWithType(EGameplayTagSourceType::RestrictedTagList, TagSources);
		UGameplayTagsManager::Get().FindTagSourcesWithType(EGameplayTagSourceType::DataTable, TagSources);

		for(auto* TagSource : TagSources)
		{
			if(UGameplayTagAccessorSettings::Get()->bUseAllTagSources == false)
			{
				if(ExplicitTagSources.Contains(TagSource->SourceName) == false)
				{
					continue;
				}
			}
			
			TArray<FGameplayTagTableRow> AllTagTableRows;
			if (TagSource->SourceType == EGameplayTagSourceType::DataTable)
			{
				UDataTable* TagTable = LoadObject<UDataTable>(nullptr, *TagSource->SourceName.ToString(), nullptr,
				                                              LOAD_None, nullptr);
				if(TagTable)
				{
					TArray<FGameplayTagTableRow*> TagTableRows;
					TagTable->GetAllRows(FString(), TagTableRows);
					for (auto* TagTableRow : TagTableRows)
					{
						AllTagTableRows.Emplace(*TagTableRow);
					}
				}
			}
			else
			{
				// for all tags in tag source, add to AllTags
				if (TagSource->SourceTagList != nullptr)
				{
					for (const auto Tag : TagSource->SourceTagList->GameplayTagList)
					{
						AllTagTableRows.Emplace(Tag);
					}
				}
				if (TagSource->SourceRestrictedTagList != nullptr)
				{
					for (const auto Tag : TagSource->SourceRestrictedTagList->RestrictedGameplayTagList)
					{
						AllTagTableRows.Emplace(Tag);
					}
				}
			}

			if(AllTagTableRows.Num() == 0)
			{
				continue;
			}
			
			header_file_content << "    /**\t\t" << TCHAR_TO_UTF8(*TagSource->SourceName.ToString()) << "\t\t**/ \n";
			for(const auto& TagRow : AllTagTableRows)
			{
				// convert . to _ from TagString
				FString TagRawString = TagRow.Tag.ToString();
				FString TagString = TagRow.Tag.ToString();
				TagString.ReplaceInline(TEXT("."), TEXT("_"), ESearchCase::CaseSensitive);
				std::string tag_name = TCHAR_TO_UTF8(*TagString);
				std::string tag_raw_name = TCHAR_TO_UTF8(*TagRawString);
				std::string tag_comment = TCHAR_TO_UTF8(*TagRow.DevComment);

				std::string name_prefix = TCHAR_TO_UTF8(*UGameplayTagAccessorSettings::Get()->NamePrefix);
				std::string tag_prefix = TCHAR_TO_UTF8(*UGameplayTagAccessorSettings::Get()->TagPrefix);

				if(UGameplayTagAccessorSettings::Get()->bUseNameAccessor && !UGameplayTagAccessorSettings::Get()->bAlternativeSortingForName)
				{
					if(TagRow.DevComment.IsEmpty() == false)
					{
						header_file_content << "    /* " << tag_comment << "*/ \n";
					}
					header_file_content << "    inline FName " << name_prefix << tag_name << " = TEXT(\"" << tag_raw_name << "\");" << "\n";
				}
				if(TagRow.DevComment.IsEmpty() == false)
				{
					header_file_content << "    /* " << tag_comment << "*/ \n";
				}
				header_file_content << "    inline FGameplayTag " << tag_prefix << tag_name << " = FGameplayTag::RequestGameplayTag(\"" <<
					tag_raw_name << "\", true);" << "\n";
			}
			if(UGameplayTagAccessorSettings::Get()->bUseNameAccessor && UGameplayTagAccessorSettings::Get()->bAlternativeSortingForName)
			{
				for(const auto& TagRow : AllTagTableRows)
				{
					// convert . to _ from TagString
					FString TagRawString = TagRow.Tag.ToString();
					FString TagString = TagRow.Tag.ToString();
					TagString.ReplaceInline(TEXT("."), TEXT("_"), ESearchCase::CaseSensitive);
					std::string tag_name = TCHAR_TO_UTF8(*TagString);
					std::string tag_raw_name = TCHAR_TO_UTF8(*TagRawString);
					std::string tag_comment = TCHAR_TO_UTF8(*TagRow.DevComment);

					std::string name_prefix = TCHAR_TO_UTF8(*UGameplayTagAccessorSettings::Get()->NamePrefix);
					
					if(TagRow.DevComment.IsEmpty() == false)
					{
						header_file_content << "    /* " << tag_comment << "*/ \n";
					}
					header_file_content << "    inline FName " << name_prefix << tag_name << " = TEXT(\"" << tag_raw_name << "\");" << "\n";
				}
			}
			header_file_content << "\n";
		}
		// ~Body
		
		// Foot
		header_file_content << "};\n";
		// ~Foot

		// if file exist, delete it
		if (FPaths::FileExists(*FullPath))
		{
			IFileManager::Get().Delete(*FullPath);
		}
		
		std::ofstream header_file(*FullPath);
		header_file << header_file_content.str();
		header_file.close();

		if(UGameplayTagAccessorSettings::Get()->bGenerateCppFile)
		{
			FString CppFullPath = Path + UGameplayTagAccessorSettings::Get()->FileName + ".cpp";
			std::string file_name = TCHAR_TO_UTF8(*UGameplayTagAccessorSettings::Get()->FileName);
			std::ostringstream cpp_file_content;
			cpp_file_content << "#include \"" << file_name << ".h\"\n";
			
			// if file exist, delete it
			if (FPaths::FileExists(*CppFullPath))
			{
				IFileManager::Get().Delete(*CppFullPath);
			}
			
			std::ofstream cpp_file(*CppFullPath);
			cpp_file << cpp_file_content.str();
			cpp_file.close();
		}

		return FPaths::FileExists(*FullPath);
	}
	catch (...)
	{
		return false;
	}
	return true;
}

void FGameplayTagAccessorModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FGameplayTagAccessorCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(
					FToolMenuEntry::InitToolBarButton(FGameplayTagAccessorCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FGameplayTagAccessorModule, GameplayTagAccessor)
