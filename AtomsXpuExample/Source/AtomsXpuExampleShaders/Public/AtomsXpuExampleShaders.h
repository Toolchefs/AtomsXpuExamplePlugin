// ===========================================================================
// Copyright (c) 2015 Toolchefs Ltd. All rights reserved.
//
// Use of this software is subject to the terms of the Toolchefs license
// agreement provided at the time of installation or download, or which
// otherwise accompanies this software in either electronic or hard copy form.
// ===========================================================================
#pragma once

#include "CoreMinimal.h"
#include "Stats/Stats.h"
#include "Modules/ModuleManager.h"

class FAtomsXpuExampleShadersShadersModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
