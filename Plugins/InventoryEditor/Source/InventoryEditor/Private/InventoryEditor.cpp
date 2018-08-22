#include "InventoryEditor.h"
#include "InventoryEditorStyle.h"

void FInventoryEditorModule::StartupModule() {
  IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

  RegisterAssetTypeAction(AssetTools, MakeShareable(new FItemDatabaseAssetActions(EAssetTypeCategories::Type::Misc)));

  // Register slate style
  FInventoryEditorStyle::Initialize();

  // TODO: Add property changed
  // OnPropertyChangedDelegateHandle = FCoreUObjectDelegates::OnObjectPropertyChanged.AddRaw(this, &FInventoryEditorModule::OnPropertyChanged);
  // FEditorDelegates::OnAssetReimport.AddRaw(this, &FInventoryEditorModule::OnObjectReimported);

  // TODO: register editor modes?
}

void FInventoryEditorModule::ShutdownModule() {

  // FCoreUObjectDelegates::OnObjectPropertyChanged.Remove(OnPropertyChangedDelegateHandle);

  if (FModuleManager::Get().IsModuleLoaded("AssetTools")) {
			IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
			for (int32 Index = 0; Index < CreatedAssetTypeActions.Num(); ++Index){
          AssetTools.UnregisterAssetTypeActions(CreatedAssetTypeActions[Index].ToSharedRef());
      }
  }
  CreatedAssetTypeActions.Empty();

  // Unregister slate style
  FInventoryEditorStyle::Shutdown();
}

void FInventoryEditorModule::RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action)
{
  AssetTools.RegisterAssetTypeActions(Action);
  CreatedAssetTypeActions.Add(Action);
}

IMPLEMENT_MODULE(FInventoryEditorModule, InventoryEditor)
