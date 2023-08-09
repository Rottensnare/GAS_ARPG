// Copyright Epic Games, Inc. All Rights Reserved.

#include "WombatManager.h"

#include "EditorAssetLibrary.h"
#include "EditorUtilityLibrary.h"
#include "WombatManagerStyle.h"
#include "WombatManagerCommands.h"
#include "LevelEditor.h"
#include "Selection.h"
#include "SWombatWindowMenu.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"

static const FName WombatManagerTabName("WombatManager");

#define LOCTEXT_NAMESPACE "FWombatManagerModule"

void FWombatManagerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FWombatManagerStyle::Initialize();
	FWombatManagerStyle::ReloadTextures();

	FWombatManagerCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FWombatManagerCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FWombatManagerModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FWombatManagerModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(WombatManagerTabName, FOnSpawnTab::CreateRaw(this, &FWombatManagerModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FWombatManagerTabTitle", "WombatManager"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FWombatManagerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FWombatManagerStyle::Shutdown();

	FWombatManagerCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(WombatManagerTabName);
}

TSharedRef<SDockTab> FWombatManagerModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FWombatManagerModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("WombatManager.cpp"))
		);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
		SNew(SWombatWindowMenu)
		];
}

void FWombatManagerModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(WombatManagerTabName);
}

TArray<FString> FWombatManagerModule::GetAllActorDataFromSelected()
{
	TArray<FString> ReturnStrings;
	if(!GEditor) return ReturnStrings;
	
	USelection* Selection = GEditor->GetSelectedActors();
	TArray<TWeakObjectPtr<UObject>> SelectedObjects;
	Selection->GetSelectedObjects(SelectedObjects);
	for(const auto Object : SelectedObjects)
	{
		if(Object.Get()->IsA<AActor>())
		{
			ReturnStrings.Add(Object->GetName());
		}
	}
	
	return ReturnStrings;
}

void FWombatManagerModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FWombatManagerCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FWombatManagerCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FWombatManagerModule, WombatManager)