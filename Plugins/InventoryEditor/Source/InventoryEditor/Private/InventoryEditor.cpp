#include "InventoryEditor.h"
#include "InventoryEditorStyle.h"
#include "InventorySimpleEditor/InventorySimpleEditorCommands.h"

#define LOCTEXT_NAMESPACE "InventoryEditor"

void FInventoryEditorModule::StartupModule() {
  IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

  RegisterAssetTypeAction(AssetTools, MakeShareable(new FItemDatabaseAssetActions(EAssetTypeCategories::Type::Misc)));
  RegisterAssetTypeAction(AssetTools, MakeShareable(new FInventorySimpleAssetActions(EAssetTypeCategories::Type::Misc)));

  RegisterSettings();
  
  // Register slate style
  FInventoryEditorStyle::Initialize();

  // Unregister commands
  FInventorySimpleEditorCommands::Register();

  // TODO: Add property changed?
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

  UnregisterSettings();

  // Unregister commands
  FInventorySimpleEditorCommands::Unregister();

  // Unregister slate style
  FInventoryEditorStyle::Shutdown();
}

void FInventoryEditorModule::RegisterSettings() {
}

void FInventoryEditorModule::UnregisterSettings() {
}

void FInventoryEditorModule::RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action)
{
  AssetTools.RegisterAssetTypeActions(Action);
  CreatedAssetTypeActions.Add(Action);
}

IMPLEMENT_MODULE(FInventoryEditorModule, InventoryEditor)

#undef LOCTEXT_NAMESPACE
