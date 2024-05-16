#pragma once

#include "CoreMinimal.h"
#include "ShaderParameters.h"
#include "ShaderParameterStruct.h"
#include "Shader.h"
#include "ShaderCompilerCore.h"
#include "GlobalShader.h"
#include "DataDrivenShaderPlatformInfo.h"
#include "Matrix3x4.h"


inline bool IsAtomsVulkanMobilePlatform(const FStaticShaderPlatform Platform)
{
	return Platform == SP_VULKAN_PCES3_1 || Platform == SP_VULKAN_ES3_1_ANDROID
#if ENGINE_MAJOR_VERSION < 5
		|| Platform == SP_VULKAN_ES3_1_LUMIN
#endif
		|| IsVulkanMobilePlatform(GMaxRHIShaderPlatform);
}

class ATOMSXPUEXAMPLESHADERS_API FAtomsXpuExampleJointTransformCS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FAtomsXpuExampleJointTransformCS);
	SHADER_USE_PARAMETER_STRUCT(FAtomsXpuExampleJointTransformCS, FGlobalShader);


	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWBuffer<float4>, CurrentBoneMatrices)
		SHADER_PARAMETER_SRV(Buffer<float4>, BindInverseMatrices)
		SHADER_PARAMETER_SRV(Buffer<int>, ParentBoneIndices)
		SHADER_PARAMETER_RDG_BUFFER_SRV(Buffer<float>, TransformData)
		SHADER_PARAMETER_RDG_BUFFER_SRV(Buffer<float4>, AgentsData)
		SHADER_PARAMETER(FMatrix3x4, ParentTransform)
		SHADER_PARAMETER(uint32, NumBones)
		SHADER_PARAMETER(uint32, NumInstances)
		SHADER_PARAMETER(uint32, AgentOffset)
		SHADER_PARAMETER(uint32, TransformDataStride)
		END_SHADER_PARAMETER_STRUCT()

		static uint32 GetGroupSize(EShaderPlatform Platform)
	{
		return 64;
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
		OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
		OutEnvironment.CompilerFlags.Add(CFLAG_AllowTypedUAVLoads);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZE"), GetGroupSize(Parameters.Platform));
		OutEnvironment.SetDefine(TEXT("ATOMS_LINEAR_UAV"), IsAtomsVulkanMobilePlatform(Parameters.Platform) ? 1 : 0);
	}

#if ENGINE_MAJOR_VERSION > 4 && ENGINE_MAJOR_VERSION > 0
#else
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return RHISupportsComputeShaders(Parameters.Platform);
	}
#endif

};