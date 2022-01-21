// ===========================================================================
// Copyright (c) 2015 Toolchefs Ltd. All rights reserved.
//
// Use of this software is subject to the terms of the Toolchefs license
// agreement provided at the time of installation or download, or which
// otherwise accompanies this software in either electronic or hard copy form.
// ===========================================================================
using UnrealBuildTool;

public class AtomsXpuExampleShaders : ModuleRules
{
	public AtomsXpuExampleShaders(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        //bUsePrecompiled = true;
        PrecompileForTargets = PrecompileTargetsType.Any;

        PublicIncludePaths.AddRange(
            new string[] {
				// ... add public include paths required here ...
                "AtomsRealtime"
			}
            );


        PrivateIncludePaths.AddRange(
            new string[] {
				// ... add other private include paths required here ...
			}
        );

        PrivateIncludePathModuleNames.AddRange(
            new string[]
            {

            }
        );

        if (Target.Type == TargetRules.TargetType.Editor)
        {
            PrivateIncludePathModuleNames.Add("UnrealEd");
        }

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "RHI",
                "RenderCore",
				// ... add other public dependencies that you statically link with here ...
			}
        );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "RHI",
                "RenderCore",
                "Projects"
				// ... add private dependencies that you statically link with here ...	
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
