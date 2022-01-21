// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "AtomsXpuExample.h"
#include "CoreMinimal.h"
#include "Containers/Array.h"
#include "Templates/SharedPointer.h"
#include "Interfaces/IPluginManager.h"
#include "ShaderCore.h"

#define LOCTEXT_NAMESPACE "FAtomsXpuExampleModule"

void FAtomsXpuExampleModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

}

void FAtomsXpuExampleModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAtomsXpuExampleModule, AtomsXpuExample)