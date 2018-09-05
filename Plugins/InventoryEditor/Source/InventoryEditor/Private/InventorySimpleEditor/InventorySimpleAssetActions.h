#pragma once

#include "CoreMinimal.h"
#include "Toolkits/IToolkitHost.h"
#include "AssetTypeActions_Base.h"

class FInventorySimpleAssetActions : public FAssetTypeActions_Base {
public:
  FInventorySimpleAssetActions(EAssetTypeCategories::Type InAssetCategory);

  // IAssetTypeActions
  virtual FText GetName() const override;
  virtual FColor GetTypeColor() const override;
  virtual UClass* GetSupportedClass() const override;
  virtual bool HasActions(const TArray<UObject*>& InObjects) const override { return true; }
  virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
  virtual uint32 GetCategories() override;

private:
  EAssetTypeCategories::Type MyAssetCategory;
};
