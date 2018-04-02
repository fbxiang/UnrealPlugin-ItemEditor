#pragma once

#include "CoreMinimal.h"
#include "ItemBase.generated.h"

UCLASS(Abstract)
class UItemBase : public UObject
{
  GENERATED_BODY()
public:
  virtual FText GetDisplayText() { return FText::GetEmpty(); };
  virtual FText GetTooltipText() { return FText::GetEmpty(); };
  virtual void GetChildren(TArray<UItemBase*>& OutChildren) { return; };
};
