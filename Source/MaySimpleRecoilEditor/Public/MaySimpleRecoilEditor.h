#pragma once

#include "Modules/ModuleManager.h"

class FMaySimpleRecoilEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};