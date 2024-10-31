#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FMarkAsNPCModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
private:
    TSharedRef<FExtender> OnExtendContentBrowserMenu(const TArray<FAssetData>& SelectedAssets);
    void AddMenuEntry(FMenuBuilder& MenuBuilder);
    void MarkAsNPCClicked();

    TArray<FAssetData> SelectedAssetData;
};
