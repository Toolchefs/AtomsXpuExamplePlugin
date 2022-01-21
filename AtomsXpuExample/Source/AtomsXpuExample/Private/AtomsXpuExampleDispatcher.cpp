#include "AtomsXpuExampleDispatcher.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "RenderGraphResources.h"
#include "AtomsXpuExampleComputeShaders.h"
#include "Components/AtomsAgentTypeComponent.h"
#include "AtomsSettings.h"

const int32 FAtomsXpuExampleJointTransformXPUComputeResources::Stride = 16;

FAtomsXpuExampleJointTransformXPUComputeResources::FAtomsXpuExampleJointTransformXPUComputeResources() : 
	FAgentPoseGPUComputeResources()
{
	
}

FAtomsXpuExampleJointTransformXPUComputeResources::~FAtomsXpuExampleJointTransformXPUComputeResources()
{
}

void FAtomsXpuExampleJointTransformXPUComputeResources::ReleaseResources()
{
	auto RenderData = this;
	ENQUEUE_RENDER_COMMAND(FAtomsXpuExampleJointTransformXPUComputeResourcesRelease)
		([RenderData](FRHICommandListImmediate& RhiCmdList)
	{
		if (RenderData->Buffer.IsValid())
		{
			RenderData->Buffer.Release();
		}
	});
}


int32 AtomsXpuExampleJointTransformXPUComputeData::staticTypeId = 999;

AtomsXpuExampleJointTransformXPUComputeData::AtomsXpuExampleJointTransformXPUComputeData() : AgentPoseGPUComputeData()
{
}

AtomsXpuExampleJointTransformXPUComputeData::~AtomsXpuExampleJointTransformXPUComputeData()
{

}

int32 AtomsXpuExampleJointTransformXPUComputeData::typeId()
{
	return staticTypeId;
}


////////////////////////////
// Dispatcher for component
////////////////////////////

int32 FAtomsXpuExampleJointTransformXPUCompute::staticTypeId = 998;

int32 FAtomsXpuExampleJointTransformXPUCompute::typeId() const
{
	return staticTypeId;
}

FAtomsXpuExampleJointTransformXPUCompute::FAtomsXpuExampleJointTransformXPUCompute() : FAgentPoseGPUCompute()
, PoseGPUComputeResources(nullptr)
{
	
}

FAtomsXpuExampleJointTransformXPUCompute::~FAtomsXpuExampleJointTransformXPUCompute()
{
	if (PoseGPUComputeResources)
	{
		PoseGPUComputeResources->ReleaseResources();
		BeginCleanup(PoseGPUComputeResources);
		PoseGPUComputeResources = nullptr;
	}
}

void FAtomsXpuExampleJointTransformXPUCompute::Update(FAtomsAgentTypeRenderObject* RenderObject, FAgentTypePoseRenderData* BonesData, TSharedPtr<AgentPoseGPUComputeData, ESPMode::ThreadSafe>& ComputeData)
{
	if (!PoseGPUComputeResources)
	{
		InitResources(RenderObject);
	}
}

void FAtomsXpuExampleJointTransformXPUCompute::Execute(
	FRHICommandListImmediate& RhiCmdList, 
	FRDGBuilder& GraphBuilder, 
	FAgentTypeRDGBuffers& AgentTypeBuffers, 
	FAtomsAgentTypeRenderObject* RenderObject, 
	FAgentTypePoseRenderData* BonesData, 
	TSharedPtr<AgentPoseGPUComputeData, 
	ESPMode::ThreadSafe>& ComputeData)
{
	if (!RenderObject || !PoseGPUComputeResources || !ComputeData || !BonesData || BonesData->AgentData.Num() == 0)
		return;

	FRDGBufferRef WorldMatricesRDGBuffer = AgentTypeBuffers.WorldMatricesRDGBuffer; // this buffer contains the current poses
	FRDGBufferRef PrevWorldMatricesRDGBuffer = AgentTypeBuffers.PrevWorldMatricesRDGBuffer; // this buffer contains the previous poses
	FRDGBufferRef InFramesBuffer = nullptr;

	TSharedPtr<AtomsXpuExampleJointTransformXPUComputeData, ESPMode::ThreadSafe> GpuComputeData =
		StaticCastSharedPtr<AtomsXpuExampleJointTransformXPUComputeData, AgentPoseGPUComputeData, ESPMode::ThreadSafe>(ComputeData);

	FAtomsXpuExampleJointTransformXPUComputeResources* JointTransformResource = static_cast<FAtomsXpuExampleJointTransformXPUComputeResources*>(PoseGPUComputeResources);
	uint32 FramesVectorArraySize = sizeof(float) * GpuComputeData->Data.Num();
	if (GpuComputeData->Data.Num() > 0 && (!JointTransformResource->Buffer.IsValid() || FramesVectorArraySize != JointTransformResource->Buffer.GetSize()))
	{
		if (JointTransformResource->Buffer.IsValid())
		{
			JointTransformResource->Buffer.Release();
		}

		const FRDGBufferDesc Desc = FRDGBufferDesc::CreateBufferDesc(sizeof(float), GpuComputeData->Data.Num());
		FRDGBufferRef InBuffer = GraphBuilder.CreateBuffer(Desc, TEXT("StateMachineXPUAgentAnimStates"), ERDGBufferFlags::MultiFrame);
		ConvertToExternalBuffer(GraphBuilder, InBuffer, JointTransformResource->Buffer.RDGBuffer);
		JointTransformResource->Buffer.SRV = JointTransformResource->Buffer.RDGBuffer->GetOrCreateSRV(FRDGBufferSRVDesc(InBuffer, PF_R32_FLOAT));
		InFramesBuffer = InBuffer;
	}
	else
	{
		InFramesBuffer = GraphBuilder.RegisterExternalBuffer(JointTransformResource->Buffer.RDGBuffer);
	}

	auto NumAgents = BonesData->AgentData.Num();
	if (InFramesBuffer && WorldMatricesRDGBuffer && PrevWorldMatricesRDGBuffer && NumAgents > 0 && GpuComputeData->Data.Num() > 0)
	{
		auto WorldMatricesUAV = GraphBuilder.CreateUAV(WorldMatricesRDGBuffer, PF_A32B32G32R32F);
		auto PrevWorldMatricesUAV = GraphBuilder.CreateUAV(PrevWorldMatricesRDGBuffer, PF_A32B32G32R32F);

		AddBufferUploadPass(GraphBuilder, InFramesBuffer, GpuComputeData->Data.GetData(), sizeof(float) * GpuComputeData->Data.Num(), ERDGInitialDataFlags::None);

		FMatrix3x4 ParentMatrix;
		FMatrix tempMtx = GpuComputeData->ActorTransform.ToMatrixWithScale();
		ParentMatrix.SetMatrixTranspose(tempMtx);

		int32 AgentGroupSize = GetDefault<UAtomsCrowdSettings>()->ComputeShaderWorkGroupSize;
		int32 NumGroups = FMath::CeilToInt(float(NumAgents) / float(AgentGroupSize));
		for (int32 groupId = 0; groupId < NumGroups; ++groupId)
		{
			TShaderMapRef<FAtomsXpuExampleJointTransformCS> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
			int32 NumAgentsInGroup = groupId == (NumGroups - 1) ? NumAgents - groupId * AgentGroupSize : AgentGroupSize;
			FAtomsXpuExampleJointTransformCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FAtomsXpuExampleJointTransformCS::FParameters>();
			PassParameters->BindInverseMatrices = RenderObject->BindBoneMatricesBuffer->VertexBufferSRV();
			PassParameters->ParentBoneIndices = RenderObject->ParentBoneIndicesBuffer->VertexBufferSRV();
			PassParameters->TransformData = GraphBuilder.CreateSRV(InFramesBuffer, EPixelFormat::PF_R32_FLOAT);
			PassParameters->NumBones = RenderObject->NumBones;
			PassParameters->NumInstances = NumAgentsInGroup;
			PassParameters->AgentOffset = AgentGroupSize * groupId;
			PassParameters->TransformDataStride = JointTransformResource->Stride;
			PassParameters->CurrentBoneMatrices = WorldMatricesUAV;
			PassParameters->ParentTransform = ParentMatrix;
			PassParameters->AgentsData = RenderObject->AgentsDataBuffer->VertexBufferSRV();
			PassParameters->SimulateOnlyActiveAgents = BonesData->SimulateOnlyActiveAgents;
			const FIntVector GroupCounts = FComputeShaderUtils::GetGroupCount(PassParameters->NumInstances, 64);
			if (PassParameters->TransformData)
				FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("ComputeShader(AtomsXpuExampleJointTransform)_%d", groupId), ComputeShader, PassParameters, GroupCounts);
		}
	}
}

void FAtomsXpuExampleJointTransformXPUCompute::ReleaseResources(FAtomsAgentTypeRenderObject* RenderObject)
{
	if (PoseGPUComputeResources)
	{
		PoseGPUComputeResources->ReleaseResources();

		// Begin a deferred delete of RenderData.  BeginCleanup will call RenderData->FinishDestroy after the above release resource
		// commands execute in the rendering thread.
		BeginCleanup(PoseGPUComputeResources);
		PoseGPUComputeResources = nullptr;
	}
}

void FAtomsXpuExampleJointTransformXPUCompute::InitResources(FAtomsAgentTypeRenderObject* RenderObject)
{
	if (!PoseGPUComputeResources)
	{
		PoseGPUComputeResources = new FAtomsXpuExampleJointTransformXPUComputeResources;
	}
}


/////////////////////////////////////////////////
// Joint Transform Dispatcher for Behaviour Tree
//////////////////////////////////////////////////

int32 FAtomsXpuExampleJointTransformXPUDispatcher::staticTypeId = 999;

int32 FAtomsXpuExampleJointTransformXPUDispatcher::typeId() const
{
	return staticTypeId;
}

FAtomsXpuExampleJointTransformXPUDispatcher::FAtomsXpuExampleJointTransformXPUDispatcher() : FAtomsBehaviourTreeXPUDispatcher(),
PoseGPUComputeResources(nullptr)
{

}

FAtomsXpuExampleJointTransformXPUDispatcher::~FAtomsXpuExampleJointTransformXPUDispatcher()
{
	ReleaseResources(nullptr);
}

void FAtomsXpuExampleJointTransformXPUDispatcher::Update(FAtomsAgentTypeRenderObject* RenderObject, FAgentTypePoseRenderData* BonesData, TSharedPtr<AgentPoseGPUComputeData, ESPMode::ThreadSafe>& ComputeData)
{
	if (!PoseGPUComputeResources)
	{
		PoseGPUComputeResources = new FBehaviourTreeXPUComputeResources;
		InitResources(RenderObject);
	}
}

void FAtomsXpuExampleJointTransformXPUDispatcher::Execute(FRHICommandListImmediate& RhiCmdList, FRDGBuilder& GraphBuilder, FAgentTypeRDGBuffers& AgentTypeBuffers, FAtomsAgentTypeRenderObject* RenderObject, FAgentTypePoseRenderData* BonesData, TSharedPtr<AgentPoseGPUComputeData, ESPMode::ThreadSafe>& ComputeData)
{
	if (!ComputeData || !BonesData || BonesData->AgentData.Num() == 0 || !RenderObject || !RenderObject->SupportsBonesBufferSRV() || !PoseGPUComputeResources)
		return;

	FRDGBufferRef WorldMatricesRDGBuffer = AgentTypeBuffers.WorldMatricesRDGBuffer;
	FRDGBufferRef PrevWorldMatricesRDGBuffer = AgentTypeBuffers.PrevWorldMatricesRDGBuffer;
	FRDGBufferRef InFramesBuffer = nullptr;

	uint32 FramesVectorArraySize = sizeof(float) * PoseGPUComputeResources->Data.Num();
	if (PoseGPUComputeResources->Data.Num() > 0 && (!PoseGPUComputeResources->Buffer.IsValid() || FramesVectorArraySize != PoseGPUComputeResources->Buffer.GetSize()))
	{
		if (PoseGPUComputeResources->Buffer.IsValid())
		{
			PoseGPUComputeResources->Buffer.Release();
		}

		const FRDGBufferDesc Desc = FRDGBufferDesc::CreateBufferDesc(sizeof(float), PoseGPUComputeResources->Data.Num());
		FRDGBufferRef InBuffer = GraphBuilder.CreateBuffer(Desc, TEXT("JointTransformXPUDispatcherData"), ERDGBufferFlags::MultiFrame);
		ConvertToExternalBuffer(GraphBuilder, InBuffer, PoseGPUComputeResources->Buffer.RDGBuffer);
		PoseGPUComputeResources->Buffer.SRV = PoseGPUComputeResources->Buffer.RDGBuffer->GetOrCreateSRV(FRDGBufferSRVDesc(InBuffer, PF_R32_FLOAT));
		InFramesBuffer = InBuffer;
	}
	else
	{
		InFramesBuffer = GraphBuilder.RegisterExternalBuffer(PoseGPUComputeResources->Buffer.RDGBuffer);
	}

	auto NumAgents = PoseGPUComputeResources->NumAgents;
	if (InFramesBuffer && NumAgents > 0 && PoseGPUComputeResources->Data.Num() > 0)
	{
		auto WorldMatricesUAV = GraphBuilder.CreateUAV(WorldMatricesRDGBuffer, PF_A32B32G32R32F);
		AddBufferUploadPass(GraphBuilder, InFramesBuffer, PoseGPUComputeResources->Data.GetData(), sizeof(float) * PoseGPUComputeResources->Data.Num(), ERDGInitialDataFlags::None);

		FMatrix3x4 ParentMatrix;
		FMatrix tempMtx = PoseGPUComputeResources->ActorTransform.ToMatrixWithScale()/*.Inverse()*/;
		ParentMatrix.SetMatrixTranspose(tempMtx);

		int32 AgentGroupSize = GetDefault<UAtomsCrowdSettings>()->ComputeShaderWorkGroupSize; // 4096;
		int32 NumGroups = FMath::CeilToInt(float(NumAgents) / float(AgentGroupSize));
		for (int32 groupId = 0; groupId < NumGroups; ++groupId)
		{
			TShaderMapRef<FAtomsXpuExampleJointTransformCS> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
			int32 NumAgentsInGroup = groupId == (NumGroups - 1) ? NumAgents - groupId * AgentGroupSize : AgentGroupSize;
			FAtomsXpuExampleJointTransformCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FAtomsXpuExampleJointTransformCS::FParameters>();
			PassParameters->BindInverseMatrices = RenderObject->BindBoneMatricesBuffer->VertexBufferSRV();
			PassParameters->ParentBoneIndices = RenderObject->ParentBoneIndicesBuffer->VertexBufferSRV();
			PassParameters->TransformData = GraphBuilder.CreateSRV(InFramesBuffer, EPixelFormat::PF_R32_FLOAT);
			PassParameters->NumBones = RenderObject->NumBones;
			PassParameters->NumInstances = NumAgentsInGroup;
			PassParameters->AgentOffset = AgentGroupSize * groupId;
			PassParameters->TransformDataStride = PoseGPUComputeResources->Stride;
			PassParameters->CurrentBoneMatrices = WorldMatricesUAV;
			PassParameters->ParentTransform = ParentMatrix;
			PassParameters->AgentsData = RenderObject->AgentsDataBuffer->VertexBufferSRV();
			PassParameters->SimulateOnlyActiveAgents = BonesData->SimulateOnlyActiveAgents;
			const FIntVector GroupCounts = FComputeShaderUtils::GetGroupCount(PassParameters->NumInstances, 64);
			if (PassParameters->TransformData)
				FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("ComputeShader(AtomsXpuExampleJointTransform)_%d", groupId), ComputeShader, PassParameters, GroupCounts);
		}
	}
}

void FAtomsXpuExampleJointTransformXPUDispatcher::ReleaseResources(FAtomsAgentTypeRenderObject* RenderObject)
{
	if (PoseGPUComputeResources)
	{
		PoseGPUComputeResources->ReleaseResources();

		// Begin a deferred delete of RenderData.  BeginCleanup will call RenderData->FinishDestroy after the above release resource
		// commands execute in the rendering thread.
		BeginCleanup(PoseGPUComputeResources);
		PoseGPUComputeResources = nullptr;
	}
}

void FAtomsXpuExampleJointTransformXPUDispatcher::InitResources(FAtomsAgentTypeRenderObject* RenderObject)
{

}

void FAtomsXpuExampleJointTransformXPUDispatcher::UpdateData(UBehaviourTree_BehaviourComponent::NodeGpuData& NodeData)
{
	if (!PoseGPUComputeResources)
	{
		PoseGPUComputeResources = new FBehaviourTreeXPUComputeResources;
		InitResources(nullptr);
	}

	PoseGPUComputeResources->NumAgents = NodeData.Data.Num() / NodeData.Stride;
	PoseGPUComputeResources->Stride = NodeData.Stride;
	PoseGPUComputeResources->Data = NodeData.Data;
	PoseGPUComputeResources->ActorTransform = NodeData.ActorTransform;
}
