#pragma once

#include "CoreMinimal.h"
#include "UObject/GCObject.h"
#include "Toolkits/IToolkitHost.h"
#include "Toolkits/AssetEditorToolkit.h"

#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "SSingleObjectDetailsPanel.h"
#include "Misc/Attribute.h"

#include "Item/Item.h"
#include "Item/ItemDatabase.h"
#include "Item/ItemBase.h"


class FItemEditor : public FAssetEditorToolkit, public FGCObject
{
public:
  FItemEditor();

  // TODO: Handle Undo

  // IToolkit interface
  virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
  // End IToolkit interface

	// FAssetEditorToolkit
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;

  UItemDatabase* GetItemDatabase() const { return ItemDatabase; } 
  UItem* GetItemBeingEdited() const { return ItemBeingEdited; }
  void SetItemBeingEdited(UItem* InItem) { ItemBeingEdited = InItem; }

  virtual void AddReferencedObjects( FReferenceCollector& Collector ) override;

  void InitItemEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, class UItemDatabase* InItemDatabase );
  
protected:
  UPROPERTY()
  UItemDatabase* ItemDatabase;

  UItem* ItemBeingEdited;

protected:
  TSharedRef<SDockTab> SpawnTab_Details(const FSpawnTabArgs& Args);
  TSharedRef<SDockTab> SpawnTab_Database(const FSpawnTabArgs& Args);

  static const FName DatabaseTabId;
  static const FName DetailsTabId;
};

