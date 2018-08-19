#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "Misc/ITransaction.h"
#include "ItemDatabase.generated.h"

extern CORE_API ITransaction* GUndo;

UCLASS()
class UItemCategory : public UObject {
  GENERATED_UCLASS_BODY()
public:
  UFUNCTION()
  FName GetCategoryName() { return CategoryName; }

  UFUNCTION()
  void SetCategoryName(const FName& NewName) { CategoryName = NewName; }

  UFUNCTION()
  void AddItem(UItem* Item) { Items.Add(Item); }

  UFUNCTION()
  void DeleteItem(UItem* Item) { Items.Remove(Item); }
  

  FText GetDisplayText() { return FText::FromName(CategoryName); }
  FText GetTooltipText() { return FText::GetEmpty(); }
  void  GetChildren(TArray<UItem*>& OutChildren) { OutChildren.Append(Items); }

protected:
  UPROPERTY()
  FName CategoryName;

  UPROPERTY()
  TArray<UItem*> Items;
};

UCLASS(Blueprintable)
class UItemDatabase : public UObject {
  GENERATED_UCLASS_BODY()

public:
  const TArray<UItemCategory*>& GetCategoryList() { return ItemCategoryList; }

  UItemCategory* AddCategory();

  void DeleteCategory(UItemCategory* Category);

  UItem* AddItemToCategory(FName CategoryName);
  UItem* AddItemToCategory(UItemCategory* Category);

  void RemoveItemFromCategory(UItemCategory* Category, UItem* Item);

private:
  UPROPERTY()
  TArray<UItemCategory*> ItemCategoryList;

  UItemCategory* FindCategory(FName CategoryName);

  void SaveToUndoHistory(FName name);
};
