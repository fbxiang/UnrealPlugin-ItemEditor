#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCanvas.h"
#include "UI/InventoryStyle.h"

class SInventorySimple : public SCompoundWidget 
{
public:
  DECLARE_DELEGATE_OneParam(FOnSlotClicked, int32);
  DECLARE_DELEGATE_ThreeParams(FOnArraySlotClicked, int32, int32, int32);

protected:
  TWeakObjectPtr<UInventorySimple> InventoryPtr;
  
  int32 SlotHovered = -1;
  int32 ArrayHovered = -1;
  int32 RowHovered = -1;
  int32 ColHovered = -1;
  
  FOnSlotClicked OnSlotClicked;
  FOnArraySlotClicked OnArraySlotClicked;
  
public:
  SLATE_BEGIN_ARGS(SInventorySimple) {}

  SLATE_STYLE_ARGUMENT(FInventoryStyleSimple, Style)
  SLATE_EVENT(FOnSlotClicked, OnSlotClicked)
  SLATE_EVENT(FOnArraySlotClicked, OnArraySlotClicked)

  SLATE_END_ARGS()

  void Construct(const FArguments& InArgs, UInventorySimple* InInventoryPtr);

  FVector2D GetDisplaySize() const;

  const FSlateBrush* GetBackgroundImage() const; 

  int32 OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const;

  virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

  virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

  virtual FVector2D ComputeDesiredSize(float) const override;
};
