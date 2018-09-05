#include "Item/Item.h"
#include "Brushes/SlateColorBrush.h"

FText UItem::GetDisplayText() {
  return DisplayName;
}

FText UItem::GetTooltipText() {
  return FText::GetEmpty();
}

FSlateBrush* UItem::GetTextureBrush() {
  return &TextureBrush;
}
