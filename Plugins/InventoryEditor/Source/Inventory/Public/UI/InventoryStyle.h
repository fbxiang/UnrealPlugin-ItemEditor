#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateWidgetStyle.h"
#include "SlateWidgetStyleContainerBase.h"
#include "InventoryStyle.generated.h"

USTRUCT(Blueprintable)
struct FInventoryStyleBase : public FSlateWidgetStyle {
  GENERATED_USTRUCT_BODY()
  
  // background image of the inventory
  UPROPERTY(EditAnywhere, Category = Appearance)
  FSlateBrush BackgroundImage;
  
  // Width of the Inventory display
  UPROPERTY(EditAnywhere, Category = Appearance)
  float Width;

  // Height of the Inventory display
  UPROPERTY(EditAnywhere, Category = Appearance)
  float Height;
};

USTRUCT(Blueprintable)
struct FInventoryStyleSimple : public FInventoryStyleBase {
  GENERATED_USTRUCT_BODY()

  FInventoryStyleSimple();

  static const FName TypeName;
  static const FInventoryStyleSimple& GetDefault();

  UPROPERTY(EditAnywhere, Category = Appearance)
  FSlateBrush SlotHoverImage;

  // Start position of InventorySimple::Slots
  UPROPERTY(EditAnywhere, EditFixedSize, Category = Appearance)
  TArray<FVector2D> SlotStartPosition;

  // Width and Height of InventorySimple::Slots
  UPROPERTY(EditAnywhere, EditFixedSize, Category = Appearance)
  TArray<FVector2D> SlotSize;

  // Start position of InventorySimple::SlotArrays
  UPROPERTY(EditAnywhere, EditFixedSize, Category = Appearance)
  TArray<FVector2D> SlotArrayStartPosition;

  // Width (Height) of a single slot in Inventory::SlotArrays
  UPROPERTY(EditAnywhere, Category = Appearance)
  float ArraySlotWidth = 0.0f;
};
