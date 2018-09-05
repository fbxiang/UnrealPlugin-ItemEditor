#include "InventorySettings.h"
#include "ISettingsModule.h"
#include "ISettingsContainer.h"
#include "ISettingsCategory.h"
#include "ISettingsSection.h"

const FName UInventorySettings::ContainerName = "Project";
const FName UInventorySettings::CategoryName = "Plugins";
const FName UInventorySettings::SectionName = "Inventory";

UItemDatabase* UInventorySettings::GetItemRegistry() {
  ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
  if (!SettingsModule) return nullptr;

  TSharedPtr<ISettingsContainer> container;
  TSharedPtr<ISettingsCategory> category;
  TSharedPtr<ISettingsSection> section;
  UInventorySettings* settings;

  if (!(container = SettingsModule->GetContainer(ContainerName))) return nullptr;
  if (!(category = container->GetCategory(CategoryName))) return nullptr;
  if (!(section = category->GetSection(SectionName))) return nullptr;
  if (!(settings = Cast<UInventorySettings>(section->GetSettingsObject()))) return nullptr;

  UItemDatabase* database;
  if ((database = Cast<UItemDatabase>(settings->ItemRegistry.ResolveObject()))) {
    return database;
  }
  if ((database = Cast<UItemDatabase>(settings->ItemRegistry.TryLoad()))) {
    return database;
  }
  UE_LOG(LogTemp, Warning, TEXT("Database Asset not found"));
  return nullptr;
}
