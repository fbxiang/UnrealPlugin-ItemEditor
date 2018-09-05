#pragma once

#include "CoreMinimal.h"
#include "InventorySimple.h"
#include "UObject/GCObject.h"
#include "InventoryItemPicker.h"


class FInventorySimpleEditor : public FAssetEditorToolkit, public FGCObject {

public:
  FInventorySimpleEditor();

  // IToolkit interface
  virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;

	// FAssetEditorToolkit
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;

  // FGCObject
  virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

  void InitInventorySimpleEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, class UInventorySimple* InInventory);

  UInventorySimple* Inventory;

  UInventoryItemPicker* InventoryItemPicker;

  bool ShowSlots = false;
protected:
  TSharedRef<SDockTab> SpawnTab_Inventory(const FSpawnTabArgs& Args);
  TSharedRef<SDockTab> SpawnTab_Details(const FSpawnTabArgs& Args);
  TSharedRef<SDockTab> SpawnTab_ItemDatabase(const FSpawnTabArgs& Args);

  TSharedPtr<class FUICommandList> CommandList;
  void SetShowSlots() { ShowSlots = !ShowSlots; }
  bool IsShowSlotsChecked() { return ShowSlots; }

  static const FName InventoryTabId;
  static const FName DetailsTabId;
  static const FName ItemDatabaseTabId;

  void OnSlotClicked(int32 Index);
  void OnArraySlotClicked(int32 Index, int32 X, int32 Y);

private:
  FVector2D ComputeDisplaySize() const;

  // Bind commands to CommandList
  void BindCommands();

  // Use CommandList to create toolbar extender
  void ExtendToolbar();
};
