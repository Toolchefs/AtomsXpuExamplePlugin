// ===========================================================================
// Copyright (c) 2015 Toolchefs Ltd. All rights reserved.
//
// Use of this software is subject to the terms of the Toolchefs license
// agreement provided at the time of installation or download, or which
// otherwise accompanies this software in either electronic or hard copy form.
// ===========================================================================
using UnrealBuildTool;

public class AtomsXpuExampleEditor : ModuleRules
{
	public AtomsXpuExampleEditor(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        //bUsePrecompiled = true;
        PrecompileForTargets = PrecompileTargetsType.Any;

         PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);

        PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
		);

        PublicIncludePathModuleNames.AddRange(
            new string[]
            {
                "AtomsRealtime",
                "AtomsUnreal",
            }
        );

        PrivateIncludePathModuleNames.AddRange(
            new string[]
            {
                "AssetTools",
                "UnrealEd",
                "AtomsRealtime",
                "AtomsUnreal",
                "MainFrame"
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
				"ContentBrowser",
                "Core",
                "CoreUObject",
                "DesktopWidgets",
                "EditorStyle",
                "Engine",
                "InputCore",
                "Projects",
                "Slate",
                "SlateCore",
                "AtomsRealtime",
                "AtomsUnreal",
                "UnrealEd"
				// ... add other public dependencies that you statically link with here ...
			}
        );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "ContentBrowser",
                "Core",
                "CoreUObject",
                "DesktopWidgets",
                "EditorStyle",
                "Engine",
                "InputCore",
                "Projects",
                "Slate",
                "SlateCore",
                "AtomsUnreal",
                "UnrealEd",
                "RenderCore",
                "RHI",
                "MeshBuilder",
                "DesktopPlatform",
				"AtomsXpuExample",
				"AtomsXpuExampleShaders"
			}
        );


        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
				// ... add any modules that your module loads dynamically here ...
            }
        );
    }
}
