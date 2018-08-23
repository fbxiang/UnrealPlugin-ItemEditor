
#include "InventoryEditorStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Interfaces/IPluginManager.h"
#include "SlateOptMacros.h"

#define IMAGE_PLUGIN_BRUSH( RelativePath, ... ) FSlateImageBrush( FInventoryEditorStyle::InContent( RelativePath, ".png" ), __VA_ARGS__ )

FString FInventoryEditorStyle::InContent(const FString& RelativePath, const ANSICHAR* Extension) {
  static FString ContentDir = IPluginManager::Get().FindPlugin(TEXT("InventoryEditor"))->GetContentDir();
  return (ContentDir / RelativePath) + Extension;
}

TSharedPtr< FSlateStyleSet > FInventoryEditorStyle::StyleSet = nullptr;
TSharedPtr< class ISlateStyle > FInventoryEditorStyle::Get() { return StyleSet; }
FName FInventoryEditorStyle::GetStyleSetName()
{
	static FName InventoryEditorStyleName(TEXT("InventoryEditorStyle"));
	return InventoryEditorStyleName;
}


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void FInventoryEditorStyle::Initialize() {

  const FVector2D Icon16x16(16.0f, 16.0f);

  if (StyleSet.IsValid()) {
    return;
  }

  StyleSet = MakeShareable(new FSlateStyleSet(GetStyleSetName()));

  StyleSet->Set("InventoryEditor.AddItemImage", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_AddItemIcon_16x"), Icon16x16));
  StyleSet->Set("InventoryEditor.RemoveItemImage", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_RemoveItemIcon_16x"), Icon16x16));
  StyleSet->Set("InventoryEditor.MoveUpImage", new IMAGE_PLUGIN_BRUSH(TEXT("Icons/icon_MoveUpIcon_16x"), Icon16x16));

  FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef IMAGE_PLUGIN_BRUSH

void FInventoryEditorStyle::Shutdown() {
  if (StyleSet.IsValid()) {
    FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
    ensure(StyleSet.IsUnique());
    StyleSet.Reset();
  }
}
