#include "ItemEditor.h"

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

  virtual TSharedRef<SWidget> PopulateSlot(TSharedRef<SWidget> PropertyEditorWidget) override {
    return SNew(SVerticalBox)
      +SVerticalBox::Slot()
      .FillHeight(1)
      [
        PropertyEditorWidget
      ];
  }
};


class SItemDatabaseRowContent : public SCompoundWidget
{
public:
  SLATE_BEGIN_ARGS(SItemDatabaseRowContent) {}
  SLATE_END_ARGS()

  public:
  void Construct(const FArguments& InArgs,
                 UItemBase* InItemBase,
                 TSharedPtr<class SItemDatabaseTabBody> InParent)
    {
      ItemBase = InItemBase;
      ParentPtr = InParent;
    
      if (InItemBase->IsA(UItemCategory::StaticClass())) {
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
                    (TAttribute<FText>::FGetter::CreateUObject(InItemBase, &UItemBase::GetDisplayText)))
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
              .OnClicked(this, &SItemDatabaseRowContent::OnAddClicked)
              .ToolTipText(LOCTEXT("AddItemTooltip", "Add a new item to the category"))
              [
                SNew(SImage)
                .Image(FEditorStyle::Get().GetBrush("Plus"))]
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
                .Image(FEditorStyle::Get().GetBrush("Minus"))]
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
                    (TAttribute<FText>::FGetter::CreateUObject(InItemBase, &UItemBase::GetDisplayText)))
            ]
          ];
      }
    }
private:
  UItemBase* ItemBase;
  TWeakPtr<class SItemDatabaseTabBody > ParentPtr;

  FReply OnDeleteClicked();
  FReply OnAddClicked();
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

    ItemTreeView = SNew(STreeView<UItemBase*>)
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
              .ToolTipText(LOCTEXT("AddItemTooltip", "Add a new item to the database")) [
                SNew(SImage)
                .Image(FEditorStyle::Get().GetBrush("Plus"))
              ]
            ]
          ]
          + SVerticalBox::Slot()
          .AutoHeight() [
            ItemTreeView.ToSharedRef()
          ]
        ]
      ];
  }
  void Tick (const FGeometry & AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override {
    SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
    RefreshView();
  }

private:

  TArray<UItemBase*> CategoryList;
  void UpdateCategoryList() {
    if (ItemEditorPtr.IsValid()) {
      CategoryList = TArray<UItemBase*>(ItemEditorPtr.Pin()->GetItemDatabase()->GetCategoryList());
    }
  }

  void RefreshView() {
    UpdateCategoryList();
    ItemTreeView->RequestTreeRefresh();
  }

  TSharedPtr< STreeView<UItemBase*> > ItemTreeView;
  
  TSharedRef<ITableRow> MakeRowNameWidget(UItemBase* InItemBase, const TSharedRef<STableViewBase>& OwnerTable) {
    return SNew(STableRow<UItemBase*>, OwnerTable)
      [
        SNew(SItemDatabaseRowContent, InItemBase, SharedThis(this))
      ];
  }

  void MakeChildren(UItemBase* InItemBase, TArray<UItemBase*>& OutChildren) {
    InItemBase->GetChildren(OutChildren);
  }

  UItemBase* ItemSelected;
  void OnRowSelectionChanged(UItemBase* InItemBase, ESelectInfo::Type InSelectInfo) {
    ItemSelected = InItemBase;
    if (InItemBase && InItemBase->IsA(UItem::StaticClass())) {
      if (ItemEditorPtr.IsValid()) {
        ItemEditorPtr.Pin()->SetItemBeingEdited(static_cast<UItem*>(InItemBase));
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

  void OnDeleteRowClicked(UItemBase* ItemBase)
    {
      if (ItemBase->IsA(UItemCategory::StaticClass())) {
        if (ItemEditorPtr.IsValid()) {
          ItemEditorPtr.Pin()->GetItemDatabase()->DeleteCategory(static_cast<UItemCategory*>(ItemBase));
        }
      } else {
        // delete item
      }
      RefreshView();
    }

  void OnAddItemClicked(UItemBase* ItemBase)
    {
      if (ItemBase->IsA(UItemCategory::StaticClass())) {
        if (ItemEditorPtr.IsValid()) {
          ItemEditorPtr.Pin()->GetItemDatabase()->AddItemToCategory(static_cast<UItemCategory*>(ItemBase));
        } 
      } else {
        // ignore
      }
      RefreshView();
    }

  void SetItemName(UItemBase* ItemBase, const FName& InName) {
    if (ItemBase && ItemBase->IsA(UItemCategory::StaticClass())) {
      static_cast<UItemCategory*>(ItemBase)->SetCategoryName(InName);
    }
    RefreshView();
  }

  friend class SItemDatabaseRowContent;
};


FReply SItemDatabaseRowContent::OnDeleteClicked() {
  if (ParentPtr.IsValid()) {
    ParentPtr.Pin()->OnDeleteRowClicked(ItemBase);
  }
  return FReply::Handled();
}

FReply SItemDatabaseRowContent::OnAddClicked() {
  if (ParentPtr.IsValid()) {
    ParentPtr.Pin()->OnAddItemClicked(ItemBase);
  }
  return FReply::Handled();
}

bool SItemDatabaseRowContent::IsSelected() {
  if (ParentPtr.IsValid()) {
    return ParentPtr.Pin()->ItemSelected == ItemBase;
  }
  return false;
}

void SItemDatabaseRowContent::OnTextCommitted(const FText& InText, ETextCommit::Type InType) {
  if (ParentPtr.IsValid()) {
    ParentPtr.Pin()->SetItemName(ItemBase, FName(*InText.ToString()));
  }
}

FItemEditor::FItemEditor() : ItemBeingEdited(nullptr) {}

void FItemEditor::InitItemEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, class UItemDatabase* InItemDatabase ) {
  ItemDatabase = InItemDatabase;
  // TODO: Register Editor Commands
  // TODO: Set up editor styles

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

  return SNew(SDockTab)
    .Label(LOCTEXT("ItemDatabaseTab_Title", "Item Database"))
    [
      SNew(SItemDatabaseTabBody, ItemEditorPtr)
    ];
}

void FItemEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
  WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_Item Editor", "Item Editor"));
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
	return LOCTEXT( "AppLabel", "Item Database Editor" );
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

#undef LOCTEXT_NAMESPACE
