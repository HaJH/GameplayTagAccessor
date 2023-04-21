# GameplayTag Native Accessor Documentation

A unreal engine plugin that automatically generates accessors for gameplay tags in the project.

## Overview
GameplayTag Native Accessor is a code plugin that automatically generates C++ variables corresponding to each tag by reading the gameplay tags used in a project. This plugin simplifies code access to gameplay tags for game developers and helps them use the gameplay tag system more efficiently.

## How to use
1. To execute the plugin, 
    a. press the 'Generate GameplayTag Accessors' button in the level editor in Unreal Editor 
    b. go to Window > 'Generate GameplayTag Accessors'.

2. Once the plugin is executed, a C++ header file is generated according to the settings in the Config.

3. The header file will contain the GameplayTag converted into the appropriate C++ Getter from the user's project.
You can change the plugin's functionality through various settings in ProjectSettings > Plugins > GameplayTagAccessor.

## Configurable variables

The plugin provides the following configurable variables:

* DirectoryPath: The directory path where the C++ header file will be generated. 

* FileName: The name of the generated C++ header file. 

* bGenerateCppFile: Whether to generate a C++ file. 

* NamespaceName: The namespace to use for the generated C++ variables. 

* bUseAllTagSources: If true, find all tags in the project and create corresponding variables. 

* ExplicitTagSources: If bUseAllTagSources is false, find tags in the specified tag sources and create corresponding variables. 

* TagPrefix: The prefix to use for the generated tag variables. 

* bUseNameAccessor: If true, create an FName variable corresponding to the tag. 

* NamePrefix: The prefix to use for the generated FName variables. 

* bAlternativeSortingForName: If true, tags are collected together with tags, and names are collected together with names. 

* bUseAutomaticGenerationWhenStartUp: If true, automatically generate tag accessors when the project starts up. 

#### Configure Example:

```
// DefaultGame.ini
[/Script/GameplayTagAccessor.GameplayTagAccessorSettings]
DirectoryPath=Source/MyModule/public/Tags/
FileName=MyTagsFile
bGenerateCppFile=True
NamespaceName=MyTags
bUseAllTagSources=False
+ExplicitTagSources=DefaultGameplayTags.ini
+ExplicitTagSources=YourExplicitTags.ini
+ExplicitTagSources=/Game/YourExplicitTagTable
+ExplicitTagSources=/Game/DT_TableSource
TagPrefix=
bUseNameAccessor=True
NamePrefix=NamePrefix_
bAlternativeSortingForName=True
bUseAutomaticGenerationWhenStartUp=False
```