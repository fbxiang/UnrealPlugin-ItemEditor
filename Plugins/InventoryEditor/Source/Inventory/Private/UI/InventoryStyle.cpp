#include "InventoryStyle.h"

FInventoryStyleSimple::FInventoryStyleSimple() {}

const FName FInventoryStyleSimple::TypeName(TEXT("FInventoryStyleSimple"));
const FInventoryStyleSimple& FInventoryStyleSimple::GetDefault() {
  static const FInventoryStyleSimple style;
  return style;
}
