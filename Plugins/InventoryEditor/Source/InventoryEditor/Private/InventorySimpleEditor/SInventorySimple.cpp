#include "SInventorySimple.h"

void SInventorySimple::Construct(const FArguments& InArgs, UInventorySimple* InInventoryPtr) {
  InventoryPtr = InInventoryPtr;
  OnSlotClicked = InArgs._OnSlotClicked;
  OnArraySlotClicked = InArgs._OnArraySlotClicked;

  ChildSlot
    [
      SNew(SCanvas)
      +SCanvas::Slot()
      .Size(TAttribute<FVector2D>(this, &SInventorySimple::GetDisplaySize))
      [
      SNew(SImage)
      .Image(this, &SInventorySimple::GetBackgroundImage)
      ]
    ];
}


FVector2D SInventorySimple::ComputeDesiredSize(float Scaling) const {
  if (InventoryPtr.IsValid()) {
    return FVector2D(InventoryPtr->Style.Width, InventoryPtr->Style.Height);
  }
  return FVector2D(0, 0);
}

FVector2D SInventorySimple::GetDisplaySize() const {
  if (InventoryPtr.IsValid()) {
    return FVector2D(InventoryPtr->Style.Width, InventoryPtr->Style.Height);
  }
  return FVector2D(0, 0);
}

const FSlateBrush* SInventorySimple::GetBackgroundImage() const {
  if (InventoryPtr.IsValid()) {
    return &InventoryPtr->Style.BackgroundImage;
  }
  return nullptr;
}


int32 SInventorySimple::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const {

  int32 newLayerId = SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, ShouldBeEnabled( bParentEnabled ));

  LayerId = FMath::Max(LayerId, newLayerId);

  if (InventoryPtr.IsValid()) {
    FInventoryStyleSimple* style = &InventoryPtr->Style;
    if (SlotHovered >= 0) {
      FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId,
        AllottedGeometry.ToPaintGeometry(
          style->SlotStartPosition[SlotHovered],
          style->SlotSize[SlotHovered]),
        &style->SlotHoverImage,
        ESlateDrawEffect::None,
        FLinearColor(1.0f, 1.0f, 1.0f, 0.7f)
      );
    }

    if (ArrayHovered >= 0) {
      FVector2D size = FVector2D(style->ArraySlotWidth, style->ArraySlotWidth);
      UItemStack* stack=InventoryPtr->GetItemStackFromSlotArray(ArrayHovered, RowHovered, ColHovered);

      TArray<FVector2D> positions;
      if (!stack) {
        positions.Add(style->SlotArrayStartPosition[ArrayHovered] +
                      size * FVector2D(ColHovered, RowHovered));
      } else {
        InventoryPtr->GetItemStackPositionsInSlotArrays(stack, positions);
      }

      for (auto& pos : positions) {
        FSlateDrawElement::MakeBox(
          OutDrawElements, LayerId,
          AllottedGeometry.ToPaintGeometry(pos, size),
          &style->SlotHoverImage,
          ESlateDrawEffect::None,
          FLinearColor(1.0f, 1.0f, 1.0f, 0.7f)
        );
      }
    }

    // draw item stacks
    TArray<UItemStack*> OutStacks;
    TArray<FVector2D> OutPositions;
    TArray<FVector2D> OutSize;
    InventoryPtr->GetItemStackPositionsAndSizes(OutStacks, OutPositions, OutSize);

    for (int32 i = 0; i < OutStacks.Num(); i++) {
      FSlateDrawElement::MakeBox(
        OutDrawElements, LayerId,
        AllottedGeometry.ToPaintGeometry(OutPositions[i], OutSize[i]),
        OutStacks[i]->Item->GetTextureBrush());
    }
  }
  return LayerId;
}

FReply SInventorySimple::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) {
  FVector2D clickPosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
  FInventoryStyleSimple* style = &InventoryPtr->Style;

  if (!InventoryPtr.IsValid()) return FReply::Unhandled();
  for (int32 i = 0;
       i < FMath::Min(style->SlotStartPosition.Num(), style->SlotSize.Num());
       ++i) {
    FVector2D localPosition = clickPosition - style->SlotStartPosition[i];
    if (localPosition.X >= 0 && localPosition.X < style->SlotSize[i].X &&
        localPosition.Y >= 0 && localPosition.Y < style->SlotSize[i].Y) {
      OnSlotClicked.ExecuteIfBound(i);
      return FReply::Handled();
    }
  }

  for (int32 i = 0; i < style->SlotArrayStartPosition.Num(); i++) {
    if (!InventoryPtr->SlotArrays[i]) continue;
    FVector2D localPosition = clickPosition - style->SlotArrayStartPosition[i];
    FVector2D gridPosition = localPosition / style->ArraySlotWidth;
    if (gridPosition.X >= 0 && gridPosition.X < InventoryPtr->SlotArrays[i]->Width &&
        gridPosition.Y >= 0 && gridPosition.Y < InventoryPtr->SlotArrays[i]->Height) {
      OnArraySlotClicked.ExecuteIfBound(i, (int32)gridPosition.Y, (int32)gridPosition.X);
      return FReply::Handled();
    }
  }
  return FReply::Unhandled();
}

FReply SInventorySimple::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) {
  SlotHovered = -1;
  ArrayHovered = -1;

  FVector2D mousePosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
  if (!InventoryPtr.IsValid()) return FReply::Unhandled();
  FInventoryStyleSimple* style = &InventoryPtr->Style;

  for (int32 i = 0; i < FMath::Min(style->SlotStartPosition.Num(), style->SlotSize.Num()); ++i) {
    FVector2D localPosition = mousePosition - style->SlotStartPosition[i];
    if (localPosition.X >= 0 && localPosition.X < style->SlotSize[i].X &&
        localPosition.Y >= 0 && localPosition.Y < style->SlotSize[i].Y) {
      SlotHovered = i;
      return FReply::Handled();
    }
  }

  for (int32 i = 0; i < style->SlotArrayStartPosition.Num(); i++) {
    if (!InventoryPtr->SlotArrays[i]) continue;

    FVector2D localPosition = mousePosition - style->SlotArrayStartPosition[i];
    FVector2D gridPosition = localPosition / style->ArraySlotWidth;

    if (gridPosition.X >= 0 && gridPosition.X < InventoryPtr->SlotArrays[i]->Width &&
        gridPosition.Y >= 0 && gridPosition.Y < InventoryPtr->SlotArrays[i]->Height) {
      ArrayHovered = i;
      RowHovered = gridPosition.Y;
      ColHovered = gridPosition.X;
      return FReply::Handled();
    }
  }
    
  return FReply::Unhandled();
}
