#include "InventorySimpleAssetActions.h"
#include "InventorySimpleEditor.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FInventorySimpleAssetActions::FInventorySimpleAssetActions(EAssetTypeCategories::Type InAssetCategory)
  : MyAssetCategory(InAssetCategory)
{}

FText FInventorySimpleAssetActions::GetName() const
{
  return LOCTEXT("FInventorySimpleAssetActionsName", "Simple Inventory");
}

FColor FInventorySimpleAssetActions::GetTypeColor() const
{
  return FColor::Green;
}

UClass* FInventorySimpleAssetActions::GetSupportedClass() const
{
  return UInventorySimple::StaticClass();
}

void FInventorySimpleAssetActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
  const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

  for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt) {
    if (UInventorySimple* inventory = Cast<UInventorySimple>(*ObjIt)) {
      TSharedRef<FInventorySimpleEditor> editor(new FInventorySimpleEditor());
      editor->InitInventorySimpleEditor(Mode, EditWithinLevelEditor, inventory);
    }
  }
}

uint32 FInventorySimpleAssetActions::GetCategories()
{
  return MyAssetCategory;
}

#undef LOCTEXT_NAMESPACE
