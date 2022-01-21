#pragma once

#include "CoreMinimal.h"
#include "ShaderParameters.h"
#include "ShaderParameterStruct.h"
#include "Shader.h"
#include "ShaderCompilerCore.h"
#include "GlobalShader.h"

class ATOMSXPUEXAMPLESHADERS_API FAtomsXpuExampleJointTransformCS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FAtomsXpuExampleJointTransformCS);
	SHADER_USE_PARAMETER_STRUCT(FAtomsXpuExampleJointTransformCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWBuffer<float4>, CurrentBoneMatrices)
		SHADER_PARAMETER_SRV(Buffer<float4>, BindInverseMatrices)
		SHADER_PARAMETER_SRV(Buffer<int>, ParentBoneIndices)
		SHADER_PARAMETER_RDG_BUFFER_SRV(Buffer<float>, TransformData)
		SHADER_PARAMETER_SRV(Buffer<float4>, AgentsData)
		SHADER_PARAMETER(FMatrix3x4, ParentTransform)
		SHADER_PARAMETER(uint32, NumBones)
		SHADER_PARAMETER(uint32, NumInstances)
		SHADER_PARAMETER(uint32, AgentOffset)
		SHADER_PARAMETER(uint32, TransformDataStride)
		SHADER_PARAMETER(uint32, SimulateOnlyActiveAgents)
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
	}

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return RHISupportsComputeShaders(Parameters.Platform);
	}

};
