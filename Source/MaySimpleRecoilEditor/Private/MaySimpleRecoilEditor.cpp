#include "MaySimpleRecoilEditor.h"
#include "MayRecoilDataCustomization.h"
#include "Core/Data/MayRecoilData.h"
#include "PropertyEditorModule.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_MODULE(FMaySimpleRecoilEditorModule, MaySimpleRecoilEditor)

void FMaySimpleRecoilEditorModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	PropertyModule.RegisterCustomClassLayout(
		UMayRecoilData::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FMayRecoilDataCustomization::MakeInstance)
	);

	PropertyModule.NotifyCustomizationModuleChanged();
}

void FMaySimpleRecoilEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomClassLayout(UMayRecoilData::StaticClass()->GetFName());
	}
}