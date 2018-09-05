#include "Inventory.h"
#include "InventorySettings.h"
#include "ISettingsModule.h"

#define LOCTEXT_NAMESPACE "Inventory"

void FInventoryModule::StartupModule() {
  RegisterSettings();
}

void FInventoryModule::ShutdownModule() {
  UnregisterSettings();
}

void FInventoryModule::RegisterSettings() {
  if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings")) {
    SettingsModule->RegisterSettings(
      UInventorySettings::ContainerName,
      UInventorySettings::CategoryName,
      UInventorySettings::SectionName,
      LOCTEXT("InventoryPluginName", "Inventory"),
      LOCTEXT("InventoryPluginDescription", "Configure Inventory Settings"),
      GetMutableDefault<UInventorySettings>());
  }
}

void FInventoryModule::UnregisterSettings() {
  if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings")) {
    SettingsModule->UnregisterSettings(
      UInventorySettings::ContainerName,
      UInventorySettings::CategoryName,
      UInventorySettings::SectionName);
  }
}

IMPLEMENT_MODULE(FInventoryModule, Inventory)

#undef LOCTEXT_NAMESPACE
