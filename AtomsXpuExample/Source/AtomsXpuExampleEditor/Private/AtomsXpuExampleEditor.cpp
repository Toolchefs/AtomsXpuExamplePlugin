// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "AtomsXpuExampleEditor.h"
#include "CoreMinimal.h"
#include "Containers/Array.h"
#include "Templates/SharedPointer.h"
#include "Interfaces/IPluginManager.h"
#include "ShaderCore.h"


#define LOCTEXT_NAMESPACE "FAtomsXpuExampleEditorModule"

void FAtomsXpuExampleEditorEditorModule::StartupModule()
{
	
}

void FAtomsXpuExampleEditorEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAtomsXpuExampleEditorEditorModule, AtomsXpuExampleEditor)