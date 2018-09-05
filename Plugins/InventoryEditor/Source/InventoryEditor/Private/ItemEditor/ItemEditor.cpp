#include "ItemEditor.h"
#include "InventoryEditorStyle.h"

#define LOCTEXT_NAMESPACE "ItemEditor"

const FName FItemEditor::DatabaseTabId("ItemEditor_DatabaseId");
const FName FItemEditor::DetailsTabId("ItemEditor_DetailsId");
const FName ItemEditorAppName = FName(TEXT("ItemEditorApp"));

class SItemPropertiesTabBody : public SSingleObjectDetailsPanel 
{
public:
  SLATE_BEGIN_ARGS(SItemPropertiesTabBody) {}
  SLATE_END_ARGS()

  private:
  TWeakPtr<class FItemEditor> ItemEditorPtr;
public:
  void Construct(const FArguments& InArgs, TSharedPtr<FItemEditor> InItemEditor) {
    SSingleObjectDetailsPanel::Construct(SSingleObjectDetailsPanel::FArguments()
                                         .HostCommandList(InItemEditor->GetToolkitCommands())
                                         .HostTabManager(InItemEditor->GetTabManager()), true, true);
    ItemEditorPtr = InItemEditor;
  }

  virtual UObject* GetObjectToObserve() const override {
    if (ItemEditorPtr.IsValid())
      return ItemEditorPtr.Pin()->GetItemBeingEdited();
    return nullptr;
  }

};


class SItemDatabaseRowContent : public SCompoundWidget
{
public:
  SLATE_BEGIN_ARGS(SItemDatabaseRowContent) {}
  SLATE_END_ARGS()

  public:
  void Construct(const FArguments& InArgs,
                 URowDataWrapper* InRowDataWrapper,
                 TSharedPtr<class SItemDatabaseTabBody> InParent)
  {
    RowDataWrapper = InRowDataWrapper;
    ParentPtr = InParent;
    
    if (InRowDataWrapper->IsA(URowDataWrapperCategory::StaticClass())) {
      ChildSlot
        [
         SNew(SHorizontalBox)
         +SHorizontalBox::Slot()
         .Padding(3.0f, 3.0f)
         .HAlign(HAlign_Fill)
         .VAlign(VAlign_Center)
         [
          SNew(SInlineEditableTextBlock)
          .Text(TAttribute<FText>::Create
                (TAttribute<FText>::FGetter::CreateUObject(InRowDataWrapper, &URowDataWrapper::GetDisplayText)))
          .IsSelected(this, &SItemDatabaseRowContent::IsSelected)
          .OnTextCommitted(this, &SItemDatabaseRowContent::OnTextCommitted)
          ]
         +SHorizontalBox::Slot()
         .AutoWidth()
         .Padding(2.0f)
         [
          SNew(SButton)
          .ButtonStyle(FEditorStyle::Get(), "HoverHintOnly")
          .ForegroundColor(FSlateColor::UseForeground())
          .HAlign(HAlign_Center)
          .VAlign(VAlign_Center)
          .OnClicked(this, &SItemDatabaseRowContent::OnMoveUpClicked)
          .ToolTipText(LOCTEXT("MoveUpCategoryTooltip", "Move this category up"))
          [
           SNew(SImage)
           .Image(FInventoryEditorStyle::Get()->GetBrush("InventoryEditor.MoveUpImage"))
           ]
          ]
         +SHorizontalBox::Slot()
         .AutoWidth()
         .Padding(2.0f)
         [
          SNew(SButton)
          .ButtonStyle(FEditorStyle::Get(), "HoverHintOnly")
          .ForegroundColor(FSlateColor::UseForeground())
          .HAlign(HAlign_Center)
          .VAlign(VAlign_Center)
          .OnClicked(this, &SItemDatabaseRowContent::OnAddClicked)
          .ToolTipText(LOCTEXT("AddItemTooltip", "Add a new item to the category"))
          [
           SNew(SImage)
           .Image(FInventoryEditorStyle::Get()->GetBrush("InventoryEditor.AddItemImage"))
           ]
          ]
         +SHorizontalBox::Slot()
         .AutoWidth()
         .Padding(2.0f)
         [
          SNew(SButton)
          .ButtonStyle(FEditorStyle::Get(), "HoverHintOnly")
          .ForegroundColor(FSlateColor::UseForeground())
          .HAlign(HAlign_Center)
          .VAlign(VAlign_Center)
          .OnClicked(this, &SItemDatabaseRowContent::OnDeleteClicked)
          .ToolTipText(LOCTEXT("RemoveCategoryTooltip", "Remove this category"))
          [
           SNew(SImage)
           .Image(FInventoryEditorStyle::Get()->GetBrush("InventoryEditor.RemoveItemImage"))
           ]
          ]
         ];
    } else {
      ChildSlot
        [
         SNew(SHorizontalBox)
         +SHorizontalBox::Slot()
         .Padding(3.0f, 3.0f)
         .HAlign(HAlign_Fill)
         .VAlign(VAlign_Center)
         [
          SNew(STextBlock)
          .Text(TAttribute<FText>::Create
                (TAttribute<FText>::FGetter::CreateUObject(InRowDataWrapper, &URowDataWrapper::GetDisplayText)))
          ]
         +SHorizontalBox::Slot()
         .AutoWidth()
         .Padding(2.0f)
         [
          SNew(SButton)
          .ButtonStyle(FEditorStyle::Get(), "HoverHintOnly")
          .ForegroundColor(FSlateColor::UseForeground())
          .HAlign(HAlign_Center)
          .VAlign(VAlign_Center)
          .OnClicked(this, &SItemDatabaseRowContent::OnMoveUpClicked)
          .ToolTipText(LOCTEXT("MoveUpItemTooltip", "Move item up"))
          [
           SNew(SImage)
           .Image(FInventoryEditorStyle::Get()->GetBrush("InventoryEditor.MoveUpImage"))
           ]
          ]
         +SHorizontalBox::Slot()
         .AutoWidth()
         .Padding(2.0f)
         [
          SNew(SButton)
          .ButtonStyle(FEditorStyle::Get(), "HoverHintOnly")
          .ForegroundColor(FSlateColor::UseForeground())
          .HAlign(HAlign_Center)
          .VAlign(VAlign_Center)
          .OnClicked(this, &SItemDatabaseRowContent::OnDeleteClicked)
          .ToolTipText(LOCTEXT("RemoveItemTooltip", "Remove this item"))
          [
           SNew(SImage)
           .Image(FInventoryEditorStyle::Get()->GetBrush("InventoryEditor.RemoveItemImage"))
           ]
          ]
         ];
    }
  }
private:
  URowDataWrapper* RowDataWrapper;
  TWeakPtr<class SItemDatabaseTabBody > ParentPtr;

  FReply OnDeleteClicked();
  FReply OnAddClicked();
  FReply OnMoveUpClicked();
  bool IsSelected();
  void OnTextCommitted(const FText& InText, ETextCommit::Type InType);
};


class SItemDatabaseTabBody : public SCompoundWidget
{
public:
  SLATE_BEGIN_ARGS(SItemDatabaseTabBody) {}
  SLATE_END_ARGS()

  private:
  TWeakPtr<FItemEditor> ItemEditorPtr;
public:
  void Construct(const FArguments& InArgs, TSharedPtr<FItemEditor> InItemEditor) {
    ItemEditorPtr = InItemEditor;
    UpdateCategoryList();

    ItemTreeView = SNew(STreeView<URowDataWrapper*>)
      .SelectionMode(ESelectionMode::Single)
      .TreeItemsSource(&CategoryList)
      .OnGenerateRow(this, &SItemDatabaseTabBody::MakeRowNameWidget)
      .OnGetChildren(this, &SItemDatabaseTabBody::MakeChildren)
      .OnSelectionChanged(this, &SItemDatabaseTabBody::OnRowSelectionChanged);

    ChildSlot
      .VAlign(VAlign_Fill)
      .HAlign(HAlign_Fill)
      [
       SNew(SScrollBox)
       .Orientation(Orient_Vertical)
       +SScrollBox::Slot()
       [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
         SNew(SHorizontalBox)
         + SHorizontalBox::Slot()
         .AutoWidth()
         .Padding(2)
         [
          SNew(SButton)
          .ButtonStyle(FEditorStyle::Get(), "HoverHintOnly")
          .ForegroundColor(FSlateColor::UseForeground())
          .HAlign(HAlign_Center)
          .VAlign(VAlign_Center)
          .OnClicked(this, &SItemDatabaseTabBody::OnAddClicked)
          .ToolTipText(LOCTEXT("AddCategoryTooltip", "Add a new category"))
          [
           SNew(SImage)
           .Image(FInventoryEditorStyle::Get()->GetBrush("InventoryEditor.AddItemImage"))
           ]
          ]
         ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
         ItemTreeView.ToSharedRef()
         ]
        ]
       ];
  }

  // void Tick (const FGeometry & AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override {
  //   SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
  // }

  void RefreshView() {
    TSet<UItemCategory*> ExpandedCategories;
    for (int32 i = 0; i < CategoryList.Num(); i++) {
      if (ItemTreeView->IsItemExpanded(CategoryList[i])) {
        ExpandedCategories.Add(static_cast<URowDataWrapperCategory*>(CategoryList[i])->Category);
      }
    }

    UpdateCategoryList();

    for (int32 i = 0; i < CategoryList.Num(); i++) {
      if (ExpandedCategories.Contains(static_cast<URowDataWrapperCategory*>(CategoryList[i])->Category)) {
        ItemTreeView->SetItemExpansion(CategoryList[i], true);
      } else {
        ItemTreeView->SetItemExpansion(CategoryList[i], false);
      }
    }

    ItemTreeView->RequestTreeRefresh();
  }

private:

  TArray<URowDataWrapper*> CategoryList;
  void UpdateCategoryList() {
    if (ItemEditorPtr.IsValid()) {

      UItemDatabase* database = ItemEditorPtr.Pin()->GetItemDatabase();
      TArray<UItemCategory*> categories = database->GetCategoryList();
      CategoryList.SetNum(categories.Num());
      for (int32 i = 0; i < categories.Num(); i++) {
        URowDataWrapperCategory* newCategory = NewObject<URowDataWrapperCategory>(database, FName(""));
        newCategory->Category = categories[i];
        CategoryList[i] = newCategory;
      }
    }
  }

  TSharedPtr< STreeView<URowDataWrapper*> > ItemTreeView;
  
  TSharedRef<ITableRow> MakeRowNameWidget(URowDataWrapper* InRowDataWrapper, const TSharedRef<STableViewBase>& OwnerTable) {
    return SNew(STableRow<URowDataWrapper*>, OwnerTable)
      [
       SNew(SItemDatabaseRowContent, InRowDataWrapper, SharedThis(this))
       ];
  }

  void MakeChildren(URowDataWrapper* InRowDataWrapper, TArray<URowDataWrapper*>& OutChildren) {
    if (InRowDataWrapper->IsA(URowDataWrapperCategory::StaticClass())) {
      TArray<UItem*> items;
      static_cast<URowDataWrapperCategory*>(InRowDataWrapper)->Category->GetChildren(items);

      if (ItemEditorPtr.IsValid()) {
        UItemDatabase* database = ItemEditorPtr.Pin()->GetItemDatabase();

        OutChildren.SetNum(items.Num());
        for (int32 i = 0; i < items.Num(); i++) {
          URowDataWrapperItem* newItem = NewObject<URowDataWrapperItem>(database, FName(""));
          newItem->Item = items[i];
          OutChildren[i] = newItem;
        }
      }
    }
  }

  URowDataWrapper* ItemSelected;
  void OnRowSelectionChanged(URowDataWrapper* InRowDataWrapper, ESelectInfo::Type InSelectInfo) {
    ItemSelected = InRowDataWrapper;
    if (InRowDataWrapper && InRowDataWrapper->IsA(URowDataWrapperItem::StaticClass())) {
      if (ItemEditorPtr.IsValid()) {
        ItemEditorPtr.Pin()->SetItemBeingEdited(static_cast<URowDataWrapperItem*>(InRowDataWrapper)->Item);
      }
    } else {
      if (ItemEditorPtr.IsValid()) {
        ItemEditorPtr.Pin()->SetItemBeingEdited(nullptr);
      }
    }
  }

  FReply OnAddClicked()
  {
    if (ItemEditorPtr.IsValid()) {
      ItemEditorPtr.Pin()->GetItemDatabase()->AddCategory();
    }
    RefreshView();
    return FReply::Handled();
  }

  void OnDeleteRowClicked(URowDataWrapper* RowDataWrapper)
  {
    if (RowDataWrapper->IsA(URowDataWrapperCategory::StaticClass())) {
      if (ItemEditorPtr.IsValid()) {
        ItemEditorPtr.Pin()->GetItemDatabase()->DeleteCategory(static_cast<URowDataWrapperCategory*>(RowDataWrapper)->Category);
      }
    } else {
      ItemEditorPtr.Pin()->GetItemDatabase()->RemoveItem(static_cast<URowDataWrapperItem*>(RowDataWrapper)->Item);
    }
    RefreshView();
  }

  void OnAddItemClicked(URowDataWrapper* RowDataWrapper)
  {
    if (RowDataWrapper->IsA(URowDataWrapperCategory::StaticClass())) {
      if (ItemEditorPtr.IsValid()) {
        ItemEditorPtr.Pin()->GetItemDatabase()->AddItemToCategory(static_cast<URowDataWrapperCategory*>(RowDataWrapper)->Category);
      } 
    } else {
      // ignore
    }
    RefreshView();
  }

  void OnMoveUpClicked(URowDataWrapper* RowDataWrapper)
  {
    if (RowDataWrapper->IsA(URowDataWrapperCategory::StaticClass())) {
      if (ItemEditorPtr.IsValid()) {
        ItemEditorPtr.Pin()->GetItemDatabase()->MoveUpCategory(static_cast<URowDataWrapperCategory*>(RowDataWrapper)->Category);
      } 
    } else {
      ItemEditorPtr.Pin()->GetItemDatabase()->MoveUpItem(static_cast<URowDataWrapperItem*>(RowDataWrapper)->Item);
    }
    RefreshView();
  }

  void SetItemName(URowDataWrapper* RowDataWrapper, const FName& InName) {
    if (RowDataWrapper && RowDataWrapper->IsA(URowDataWrapperCategory::StaticClass())) {
      static_cast<URowDataWrapperCategory*>(RowDataWrapper)->Category->SetCategoryName(InName);
    }
    RefreshView();
  }

  friend class SItemDatabaseRowContent;
};


FReply SItemDatabaseRowContent::OnDeleteClicked() {
  if (ParentPtr.IsValid()) {
    ParentPtr.Pin()->OnDeleteRowClicked(RowDataWrapper);
  }
  return FReply::Handled();
}

FReply SItemDatabaseRowContent::OnAddClicked() {
  if (ParentPtr.IsValid()) {
    ParentPtr.Pin()->OnAddItemClicked(RowDataWrapper);
  }
  return FReply::Handled();
}

FReply SItemDatabaseRowContent::OnMoveUpClicked() {
  if (ParentPtr.IsValid()) {
    ParentPtr.Pin()->OnMoveUpClicked(RowDataWrapper);
  }
  return FReply::Handled();
}

bool SItemDatabaseRowContent::IsSelected() {
  if (ParentPtr.IsValid()) {
    return ParentPtr.Pin()->ItemSelected == RowDataWrapper;
  }
  return false;
}

void SItemDatabaseRowContent::OnTextCommitted(const FText& InText, ETextCommit::Type InType) {
  if (ParentPtr.IsValid()) {
    ParentPtr.Pin()->SetItemName(RowDataWrapper, FName(*InText.ToString()));
  }
}

FItemEditor::FItemEditor() : ItemBeingEdited(nullptr) {
  GEditor->RegisterForUndo(this);
}

void FItemEditor::InitItemEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, class UItemDatabase* InItemDatabase ) {
  ItemDatabase = InItemDatabase;
  // TODO: Register Editor Commands

  const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_ItemEditor_Layout_v1")
    ->AddArea
    (
     FTabManager::NewPrimaryArea()
     ->SetOrientation(Orient_Vertical)
     ->Split
     (
      FTabManager::NewStack()
      ->SetSizeCoefficient(0.1f)
      ->SetHideTabWell(true)
      ->AddTab(GetToolbarTabId(), ETabState::OpenedTab)
      )
     ->Split
     (
      FTabManager::NewSplitter()
      ->SetOrientation(Orient_Horizontal)
      ->SetSizeCoefficient(0.9f)
      ->Split
      (
       FTabManager::NewStack()
       ->SetSizeCoefficient(0.8f)
       ->SetHideTabWell(true)
       ->AddTab(FItemEditor::DatabaseTabId, ETabState::OpenedTab)
       )
      ->Split
      (
       FTabManager::NewStack()
       ->SetSizeCoefficient(0.2f)
       ->SetHideTabWell(true)
       ->AddTab(FItemEditor::DetailsTabId, ETabState::OpenedTab)
       )
      )
     );

  InitAssetEditor(Mode, InitToolkitHost, ItemEditorAppName, StandaloneDefaultLayout, true, true, InItemDatabase);
  // extend things here
}

TSharedRef<SDockTab> FItemEditor::SpawnTab_Details(const FSpawnTabArgs& Args)
{
  TSharedPtr<FItemEditor> ItemEditorPtr = SharedThis(this);

  return SNew(SDockTab)
    .Label(LOCTEXT("DetailsTab_Title", "Item Details"))
    [
     SNew(SItemPropertiesTabBody, ItemEditorPtr)
     ];
}

TSharedRef<SDockTab> FItemEditor::SpawnTab_Database(const FSpawnTabArgs& Args)
{
  TSharedPtr<FItemEditor> ItemEditorPtr = SharedThis(this);
  DatabaseTabBodyPtr = SNew(SItemDatabaseTabBody, ItemEditorPtr);

  return SNew(SDockTab)
    .Label(LOCTEXT("ItemDatabaseTab_Title", "Item Database"))
    [
     DatabaseTabBodyPtr.ToSharedRef()
     ];
}

void FItemEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
  WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_ItemEditor", "Item Editor"));
  FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

  InTabManager->RegisterTabSpawner(DatabaseTabId, FOnSpawnTab::CreateSP(this, &FItemEditor::SpawnTab_Database))
    .SetDisplayName(LOCTEXT("ItemDatabaseTab", "Item Database"))
    .SetGroup(WorkspaceMenuCategory.ToSharedRef());

  InTabManager->RegisterTabSpawner(DetailsTabId, FOnSpawnTab::CreateSP(this, &FItemEditor::SpawnTab_Details))
    .SetDisplayName(LOCTEXT("ItemDetailsTab", "Item Details"))
    .SetGroup(WorkspaceMenuCategory.ToSharedRef());
}

void FItemEditor::UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
  InTabManager->UnregisterTabSpawner(DatabaseTabId);
  InTabManager->UnregisterTabSpawner(DetailsTabId);
}

FName FItemEditor::GetToolkitFName() const
{
	return FName("ItemDatabaseEditor");
}

FText FItemEditor::GetBaseToolkitName() const
{
	return LOCTEXT( "Editor", "Item Database Editor" );
}

FString FItemEditor::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "Item Database ").ToString();
}

FLinearColor FItemEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor( 0.0f, 0.0f, 0.2f, 0.5f );
}

void FItemEditor::AddReferencedObjects( FReferenceCollector& Collector )
{
  Collector.AddReferencedObject(ItemDatabase);
}

void FItemEditor::PostUndo(bool bSuccess) {
  if (DatabaseTabBodyPtr.IsValid()) {
    DatabaseTabBodyPtr->RefreshView();
  }
}

void FItemEditor::PostRedo(bool bSuccess) {
  if (DatabaseTabBodyPtr.IsValid()) {
    DatabaseTabBodyPtr->RefreshView();
  }
}

#undef LOCTEXT_NAMESPACE
