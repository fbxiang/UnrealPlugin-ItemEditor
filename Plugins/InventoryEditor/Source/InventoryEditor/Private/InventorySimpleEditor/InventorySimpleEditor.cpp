#include "InventorySimpleEditor.h"
#include "SSingleObjectDetailsPanel.h"
#include "Widgets/SCanvas.h"
#include "UI/InventoryStyle.h"
#include "InventorySimpleEditor/InventorySimpleEditorCommands.h"
#include "Widgets/Input/SSearchBox.h"
#include "SInventorySimple.h"
#include "Widgets/Layout/SScrollBox.h"


#define LOCTEXT_NAMESPACE "InventorySimpleEditor"

class SDetailsPanel;
// class SDetailsPanelInventory;
// class SDetailsPanelOptions;

const FName FInventorySimpleEditor::InventoryTabId("InventorySimpleEditor_InventoryId");
const FName FInventorySimpleEditor::DetailsTabId("InventorySimpleEditor_Detailsid");
const FName FInventorySimpleEditor::ItemDatabaseTabId("InventorySimpleEditor_ItemDatabaseId");
const FName InventorySimpleEditorAppName = FName(TEXT("InventorySimpleEditorApp"));

FInventorySimpleEditor::FInventorySimpleEditor() : CommandList(new FUICommandList) {
}

// IToolkit
void FInventorySimpleEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) {
  WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_InventorySimpleEditor", "Simple Inventory Editor"));
  FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

  InTabManager->RegisterTabSpawner(InventoryTabId, FOnSpawnTab::CreateSP(this, &FInventorySimpleEditor::SpawnTab_Inventory))
    .SetDisplayName(LOCTEXT("InventoryTab", "Inventory"))
    .SetGroup(WorkspaceMenuCategory.ToSharedRef());

  InTabManager->RegisterTabSpawner(DetailsTabId, FOnSpawnTab::CreateSP(this, &FInventorySimpleEditor::SpawnTab_Details))
    .SetDisplayName(LOCTEXT("DetailsTab", "Details"))
    .SetGroup(WorkspaceMenuCategory.ToSharedRef());

  InTabManager->RegisterTabSpawner(ItemDatabaseTabId, FOnSpawnTab::CreateSP(this, &FInventorySimpleEditor::SpawnTab_ItemDatabase))
    .SetDisplayName(LOCTEXT("ItemDatabaseTab", "Item Database"))
    .SetGroup(WorkspaceMenuCategory.ToSharedRef());

}

void FInventorySimpleEditor::UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) {
  InTabManager->UnregisterTabSpawner(InventoryTabId);
  InTabManager->UnregisterTabSpawner(DetailsTabId);
  FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
}

// FAssetEditorToolkit
FName FInventorySimpleEditor::GetToolkitFName() const {
  return FName("InventorySimpleEditor");
}

FText FInventorySimpleEditor::GetBaseToolkitName() const {
  return LOCTEXT("Editor", "Simple Inventory Editor");
}

FString FInventorySimpleEditor::GetWorldCentricTabPrefix() const {
  return LOCTEXT("WorldCentricTabPrefix", "Simple Inventory ").ToString();
}

FLinearColor FInventorySimpleEditor::GetWorldCentricTabColorScale() const {
  return FLinearColor(0.0f, 0.0f, 0.2f, 0.5f);
}

// FGCObject
void FInventorySimpleEditor::AddReferencedObjects( FReferenceCollector& Collector ) {
  Collector.AddReferencedObject(Inventory);
}

void FInventorySimpleEditor::InitInventorySimpleEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, class UInventorySimple* InInventory) {
  Inventory = InInventory;

  // create item picker for accessing global database
  InventoryItemPicker = NewObject<UInventoryItemPicker>(Inventory);
  InventoryItemPicker->SetSearchKey(FText::FromString(""));
  InventoryItemPicker->StartSearch();

  BindCommands();

  const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_InventorySimpleEditorLayout_v1")
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
       ->SetSizeCoefficient(0.4f)
       ->SetHideTabWell(true)
       ->AddTab(FInventorySimpleEditor::InventoryTabId, ETabState::OpenedTab)
       )
      ->Split
      (
       FTabManager::NewStack()
       ->SetSizeCoefficient(0.3f)
       ->SetHideTabWell(true)
       ->AddTab(FInventorySimpleEditor::ItemDatabaseTabId, ETabState::OpenedTab)
       )
      ->Split
      (
       FTabManager::NewStack()
       ->SetSizeCoefficient(0.3f)
       ->SetHideTabWell(true)
       ->AddTab(FInventorySimpleEditor::DetailsTabId, ETabState::OpenedTab)
       )
      )
     );


  InitAssetEditor(Mode, InitToolkitHost, InventorySimpleEditorAppName, StandaloneDefaultLayout, true, true, InInventory);

  ExtendToolbar();
  RegenerateMenusAndToolbars();
}

void FInventorySimpleEditor::ExtendToolbar() {
  struct Local {
    static void FillToolbar(FToolBarBuilder& ToolbarBuilder) {
      const FInventorySimpleEditorCommands& Commands = FInventorySimpleEditorCommands::Get();

      ToolbarBuilder.BeginSection("Command");
      ToolbarBuilder.AddToolBarButton(Commands.SetShowSlots);
      ToolbarBuilder.EndSection();
    }
  };

  TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);

  ToolbarExtender->AddToolBarExtension("Asset", EExtensionHook::After, CommandList,
                                       FToolBarExtensionDelegate::CreateStatic(&Local::FillToolbar));

  AddToolbarExtender(ToolbarExtender);
}

void FInventorySimpleEditor::BindCommands() {
  const FInventorySimpleEditorCommands& Commands = FInventorySimpleEditorCommands::Get();
  CommandList->MapAction(Commands.SetShowSlots,
                         FExecuteAction::CreateSP(this, &FInventorySimpleEditor::SetShowSlots),
                         FCanExecuteAction(),
                         FIsActionChecked::CreateSP(this, &FInventorySimpleEditor::IsShowSlotsChecked));
}

class SDetailsPanel : public SSingleObjectDetailsPanel {
public:
  SLATE_BEGIN_ARGS(SDetailsPanel) {}
  SLATE_END_ARGS()

public:
  void Construct(const FArguments& InArgs, TSharedPtr<FInventorySimpleEditor> InEditorPtr) {
    SSingleObjectDetailsPanel::Construct(SSingleObjectDetailsPanel::FArguments()
                                         .HostCommandList(InEditorPtr->GetToolkitCommands())
                                         .HostTabManager(InEditorPtr->GetTabManager()), true, true);
    EditorPtr = InEditorPtr;
  }

  virtual UObject* GetObjectToObserve() const override {
    if (EditorPtr.IsValid()) {
      return EditorPtr.Pin()->Inventory;
    }
    return nullptr;
  }

private:
  TWeakPtr<FInventorySimpleEditor> EditorPtr;
};

class SInventoryEditor : public SInventorySimple 
{

protected:
  TWeakPtr<FInventorySimpleEditor> EditorPtr;
public:
  SLATE_BEGIN_ARGS(SInventoryEditor) {}

  SLATE_STYLE_ARGUMENT(FInventoryStyleSimple, Style)
  SLATE_EVENT(FOnSlotClicked, OnSlotClicked)
  SLATE_EVENT(FOnArraySlotClicked, OnArraySlotClicked)

  SLATE_END_ARGS()

  void Construct(const FArguments& InArgs, TSharedPtr<FInventorySimpleEditor> InEditorPtr) {
    EditorPtr = InEditorPtr;
    SInventorySimple::Construct(SInventorySimple::FArguments()
                                .Style(InArgs._Style)
                                .OnSlotClicked(InArgs._OnSlotClicked)
                                .OnArraySlotClicked(InArgs._OnArraySlotClicked),
                                InEditorPtr->Inventory);
  }

  int32 OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const {
    if (!EditorPtr.IsValid()) return LayerId;
    if (!InventoryPtr.IsValid()) return LayerId;
    LayerId = SInventorySimple::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, ShouldBeEnabled( bParentEnabled ));

    static const FSlateBrush* SlotBorder = FInventoryEditorStyle::Get()->GetBrush("InventoryEditor.SlotBorder");
    
    if (EditorPtr.Pin()->ShowSlots) {
      UInventorySimple* inventory = InventoryPtr.Get();
      if (!inventory) return LayerId;

      FInventoryStyleSimple* style = &InventoryPtr->Style;
      
      for (int32 i = 0; i < FMath::Min(style->SlotStartPosition.Num(), style->SlotSize.Num()); i++) {
        FSlateDrawElement::MakeBox(
          OutDrawElements,
          LayerId,
          AllottedGeometry.ToPaintGeometry(style->SlotStartPosition[i], style->SlotSize[i]),
          SlotBorder,
          ESlateDrawEffect::None,
          FLinearColor(0.0f, 1.0f, 0.0f, 0.7f)
        );
      }

      for (int32 i = 0; i < style->SlotArrayStartPosition.Num(); i++) {
        if (!inventory->SlotArrays[i]) continue;

        FSlateDrawElement::MakeBox(
          OutDrawElements,
          LayerId,
          AllottedGeometry.ToPaintGeometry(
            style->SlotArrayStartPosition[i],
            FVector2D(inventory->SlotArrays[i]->Width * style->ArraySlotWidth,
                      inventory->SlotArrays[i]->Height * style->ArraySlotWidth)),
          SlotBorder,
          ESlateDrawEffect::None,
          FLinearColor(0.0f, 1.0f, 0.0f, 0.7f)
        );
      }
    }
    return LayerId;
  }
};

void FInventorySimpleEditor::OnSlotClicked(int32 Index) {
  UE_LOG(LogTemp, Warning, TEXT("Slot Clicked %d"), Index);

}

void FInventorySimpleEditor::OnArraySlotClicked(int32 Index, int32 X, int32 Y) {
  UE_LOG(LogTemp, Warning, TEXT("Slot Clicked %d %d %d"), Index, X, Y);
}

class SInventoryItemDatabaseTabBody : public SCompoundWidget 
{
public:
  SLATE_BEGIN_ARGS(SInventoryItemDatabaseTabBody) {}
  SLATE_END_ARGS()
  
  private:
  TWeakPtr<FInventorySimpleEditor> EditorPtr;
public:
  void Construct(const FArguments& InArgs, TSharedPtr<FInventorySimpleEditor> InEditorPtr) {
    EditorPtr = InEditorPtr;
    InEditorPtr->InventoryItemPicker->Style.BackgroundImage = *FInventoryEditorStyle::Get()->GetBrush("InventoryEditor.ItemPickerBackground");

    ChildSlot
      .VAlign(VAlign_Fill)
      .HAlign(HAlign_Fill)
      [
        SNew(SVerticalBox)
        +SVerticalBox::Slot()
        .AutoHeight()
        .VAlign(VAlign_Top)
        .HAlign(HAlign_Fill)
        .Padding(10, 10)
        [
          SNew(SSearchBox)
          .HintText(LOCTEXT("ItemDatabaseSearch", "Search Item Database"))
          .DelayChangeNotificationsWhileTyping(true)
        ]
        +SVerticalBox::Slot()
        .VAlign(VAlign_Fill)
        .HAlign(HAlign_Fill)
        [
          SNew(SScrollBox)
          .Orientation(Orient_Vertical)
          +SScrollBox::Slot()
          [
            SNew(SInventorySimple, InEditorPtr->InventoryItemPicker)
            .Style(&InEditorPtr->InventoryItemPicker->Style)
          ]
        ]
      ];
  }
};


TSharedRef<SDockTab> FInventorySimpleEditor::SpawnTab_Inventory(const FSpawnTabArgs& Args) {
  return SNew(SDockTab)
    .Label(LOCTEXT("InventoryTab_Title", "Inventory"))
    [
     SNew(SCanvas)
     +SCanvas::Slot()
     .Position(FVector2D(0, 0))
     .Size(TAttribute<FVector2D>(this, &FInventorySimpleEditor::ComputeDisplaySize))
     .VAlign(VAlign_Top)
     .HAlign(HAlign_Left)
     [
      SNew(SInventoryEditor, SharedThis(this))
      .Style(&Inventory->Style)
      .OnSlotClicked(this, &FInventorySimpleEditor::OnSlotClicked)
      .OnArraySlotClicked(this, &FInventorySimpleEditor::OnArraySlotClicked)
      ]
     ];
}

TSharedRef<SDockTab> FInventorySimpleEditor::SpawnTab_Details(const FSpawnTabArgs& Args) {
  return SNew(SDockTab)
    .Label(LOCTEXT("DetailsTab_Title", "Details"))
    [
     SNew(SDetailsPanel, SharedThis(this))
     ];
}

TSharedRef<SDockTab> FInventorySimpleEditor::SpawnTab_ItemDatabase(const FSpawnTabArgs& Args) {
  return SNew(SDockTab)
    .Label(LOCTEXT("ItemDatabaseTab_Title", "Item Database"))
    [
     SNew(SInventoryItemDatabaseTabBody, SharedThis(this))
     ];
}

FVector2D FInventorySimpleEditor::ComputeDisplaySize() const {
  if (!Inventory) return FVector2D(0, 0);
  return FVector2D(Inventory->Style.Width, Inventory->Style.Height);
}

#undef LOCTEXT_NAMESPACE
