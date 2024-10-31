#include "MarkAsNPC.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Misc/MessageDialog.h"
#include "Editor.h"

#define LOCTEXT_NAMESPACE "FMarkAsNPCModule"

void FMarkAsNPCModule::StartupModule()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

	ContentBrowserModule.GetAllAssetViewContextMenuExtenders().Add(
		FContentBrowserMenuExtender_SelectedAssets::CreateRaw(this, &FMarkAsNPCModule::OnExtendContentBrowserMenu)
	);
}

void FMarkAsNPCModule::ShutdownModule()
{
	SelectedAssetData.Empty();
}

TSharedRef<FExtender> FMarkAsNPCModule::OnExtendContentBrowserMenu(const TArray<FAssetData>& SelectedAssets)
{
	SelectedAssetData = SelectedAssets;
	TSharedRef<FExtender> Extender = MakeShareable(new FExtender());

	Extender->AddMenuExtension(
		"GetAssetActions",
		EExtensionHook::After,
		nullptr,
		FMenuExtensionDelegate::CreateRaw(this, &FMarkAsNPCModule::AddMenuEntry)
	);

	return Extender;
}

void FMarkAsNPCModule::AddMenuEntry(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.BeginSection("NPCForge", FText::FromString("NPCForge"));
	{
		MenuBuilder.AddMenuEntry(
			FText::FromString("Mark as NPC"),
			FText::FromString("Make the entity an NPC"),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateRaw(this, &FMarkAsNPCModule::MarkAsNPCClicked))
		);
	}
	MenuBuilder.EndSection();
}

void FMarkAsNPCModule::MarkAsNPCClicked()
{
	// Check if there is a selected asset
	if (SelectedAssetData.Num() > 0)
	{
		// Get first selected asset
		const FAssetData& AssetData = SelectedAssetData[0];
		FString AssetName = AssetData.AssetName.ToString();
		FString AssetPath = AssetData.GetObjectPathString();
        
		UE_LOG(LogTemp, Warning, TEXT("Selected Asset: %s"), *AssetName);
		UE_LOG(LogTemp, Warning, TEXT("Asset Path: %s"), *AssetPath);

		// Clear selection
		SelectedAssetData.Empty();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No asset selected"));
	}
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FMarkAsNPCModule, MarkAsNPC)