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
#include "ItemEditor.generated.h"

class FItemEditor : public FAssetEditorToolkit, public FGCObject, public FEditorUndoClient
{
public:
  FItemEditor();

  // IToolkit interface
  virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;

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
  UItemDatabase* ItemDatabase;

  UItem* ItemBeingEdited;

  // FEditorUndoClient
  virtual void PostRedo(bool bSuccess) override;
  virtual void PostUndo(bool bSuccess) override;

protected:
  TSharedRef<SDockTab> SpawnTab_Details(const FSpawnTabArgs& Args);
  TSharedRef<SDockTab> SpawnTab_Database(const FSpawnTabArgs& Args);


  TSharedPtr<class SItemDatabaseTabBody> DatabaseTabBodyPtr;

  static const FName DatabaseTabId;
  static const FName DetailsTabId;
};


UCLASS()
class URowDataWrapper : public UObject {
  GENERATED_BODY()

  public:
  virtual FText GetDisplayText() { return FText::GetEmpty(); }
};

UCLASS()
class URowDataWrapperItem : public URowDataWrapper {
  GENERATED_BODY()
  public:
  UItem* Item;

  virtual FText GetDisplayText() {
    if (Item) {
      return FText::Format(FTextFormat::FromString("{0} ({1})"), Item->DisplayName, FText::FromName(Item->UniqueName));
    }
    return FText::GetEmpty();
  }
};

UCLASS()
class URowDataWrapperCategory : public URowDataWrapper {
  GENERATED_BODY()

  public:
  UItemCategory* Category;

  virtual FText GetDisplayText() {
    if (Category) {
      return Category->GetDisplayText();
    }
    return FText::GetEmpty();
  }
};
