#include "ItemEditor/ItemDatabaseAssetActions.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FItemDatabaseAssetActions::FItemDatabaseAssetActions(EAssetTypeCategories::Type InAssetCategory)
  : MyAssetCategory(InAssetCategory)
{}

FText FItemDatabaseAssetActions::GetName() const
{
  return LOCTEXT("FItemDatabaseAssetActionsName", "Item Database");
}

FColor FItemDatabaseAssetActions::GetTypeColor() const
{
  return FColor::Green;
}

UClass* FItemDatabaseAssetActions::GetSupportedClass() const
{
  return UItemDatabase::StaticClass();
}

void FItemDatabaseAssetActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
  const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

  for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt) {
    if (UItemDatabase* ItemDatabase = Cast<UItemDatabase>(*ObjIt)) {
      TSharedRef<FItemEditor> NewItemEditor(new FItemEditor());
      NewItemEditor->InitItemEditor(Mode, EditWithinLevelEditor, ItemDatabase);
    }
  }
}

uint32 FItemDatabaseAssetActions::GetCategories()
{
  return MyAssetCategory;
}

#undef LOCTEXT_NAMESPACE
