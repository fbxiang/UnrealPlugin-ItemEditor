#include "InventoryItemPicker.h"
#include "ISettingsModule.h"
#include "ISettingsContainer.h"
#include "ISettingsCategory.h"
#include "ISettingsSection.h"
#include "InventorySettings.h"
#include "InventoryEditorStyle.h"

UInventoryItemPicker::UInventoryItemPicker(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{
  Slots.SetNum(0);
  SlotArrays.SetNum(1);
  SlotArrays[0] = CreateDefaultSubobject<UInventorySlotArray>(TEXT("SlotArray"));
  SlotArrays[0]->Width = 8;
  SlotArrays[0]->Height = 16;
  SynchronizeStyle();
  Style.Width = 512;
  Style.Height = 1024;
  Style.SlotArrayStartPosition[0] = FVector2D(0.0f, 0.0f);
  Style.ArraySlotWidth = 64.0f;
  Style.SlotHoverImage = FSlateColorBrush(FLinearColor(1.0f, 1.0f, 1.0f, 0.6f));
}

void UInventoryItemPicker::SetSearchKey(const FText& Key) {
  SearchKey = Key;
}

void UInventoryItemPicker::StartSearch() {
  GenerateItemStackList();
  GeneratePages();
  SetPage(0);
}

void UInventoryItemPicker::GenerateItemStackList() {
  ItemStacks.SetNum(0);

  UItemDatabase* registry = UInventorySettings::GetItemRegistry();

  if (registry) {
    TArray<UItem*> ItemList;
    registry->SearchItems(SearchKey, ItemList);
    for (int32 i = 0; i < ItemList.Num(); i++) {
      UItemStack* newStack = NewObject<UItemStack>(this);
      newStack->Item = ItemList[i];
      ItemStacks.Add(newStack);
    }
  }
  UE_LOG(LogTemp, Warning, TEXT("Found %d items in the database"), ItemStacks.Num());
}

void UInventoryItemPicker::GeneratePages() {
  PageIndices.SetNum(0);
  PageIndices.Add(0);

  int32 row = 0, col = 0, maxHeight = 0;
  for (int32 i = 0; i < ItemStacks.Num(); i++) {
    int32 width = ItemStacks[i]->Item->Width;
    int32 height = ItemStacks[i]->Item->Height;

    if (width <= 0 || width >= SlotArrays[0]->Width ||
        height <= 0 || height >= SlotArrays[0]->Height) {
      // User error, item too large
      continue;
    }

    // start new line
    if (col + width > SlotArrays[0]->Width) {
      row += maxHeight; col = 0; maxHeight = 0;
    }
    if (row >= SlotArrays[0]->Height) {
      row = 0; col = 0; maxHeight = 0;
      PageIndices.Add(i);
    }
    col += width; maxHeight = FMath::Max(maxHeight, height);
  }
}

int32 UInventoryItemPicker::NumPages() {
  return PageIndices.Num();
}

void UInventoryItemPicker::FillPage() {
  int32 row = 0, col = 0, maxHeight = 0;
  int32 maxIndex = Page + 1 >= PageIndices.Num() ? ItemStacks.Num() : PageIndices[Page + 1];
  UE_LOG(LogTemp, Warning, TEXT("Fill page with items from %d to %d"), PageIndices[Page], maxIndex);

  for (int32 i = PageIndices[Page]; i < maxIndex; i++) {
    int32 width = ItemStacks[i]->Item->Width;
    int32 height = ItemStacks[i]->Item->Height;

    if (width <= 0 || width >= SlotArrays[0]->Width ||
        height <= 0 || height >= SlotArrays[0]->Height) {
      UE_LOG(LogTemp, Warning, TEXT("Item size too large"));
      continue;
    }

    // start new line
    if (col + width > SlotArrays[0]->Width) {
      row += maxHeight; col = 0; maxHeight = 0;
    }

    if (row >= SlotArrays[0]->Height) {
      UE_LOG(LogTemp, Warning, TEXT("UInventoryItemPicker::FillPage() This should never happen"));
      return;
    }

    UInventorySimple::PlaceItemStackIntoArray(ItemStacks[i], 0, row, col);
    col += width; maxHeight = FMath::Max(maxHeight, height);
  }
}

int32 UInventoryItemPicker::GetPage() {
  return Page;
}

void UInventoryItemPicker::SetPage(int32 InPage) {
  UE_LOG(LogTemp, Warning, TEXT("Set Page to %d of %d"), InPage, NumPages());

  if (InPage < 0 || InPage >= NumPages())
    return;

  Page = InPage;
  FillPage();
}

void UInventoryItemPicker::GenerateItemStackListIncremental() {
  // TODO: more efficient algorithm (if needed)
  GenerateItemStackList();
}

UItemStack* UInventoryItemPicker::RemoveItemStackFromSlot(int32 Index) {
  if (!CheckSlot(Index)) return nullptr;
  return Slots[Index] -> ItemStack;
}

UItemStack* UInventoryItemPicker::RemoveItemStackFromArray(int32 ArrayIndex, int32 X, int32 Y) {
  if (!CheckArray(ArrayIndex, X, Y)) return nullptr;

  UInventorySlotArray* array = SlotArrays[ArrayIndex];
  UItemStack* stackToRemove = array->GetItemStackAt(X, Y);

  // we remove but actually do nothing

  return stackToRemove;
}
