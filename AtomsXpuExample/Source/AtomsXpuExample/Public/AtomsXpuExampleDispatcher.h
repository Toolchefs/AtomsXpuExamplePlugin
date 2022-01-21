#pragma once
#include "XPUCompute/BehaviourTree/ComputeData.h"

class FAtomsAgentTypeRenderObject;
class UAtomsXpuExampleJointTransform_BehaviourComponent;

/*  Class containing the resources allocated on the gpu
*/
class FAtomsXpuExampleJointTransformXPUComputeResources : public FAgentPoseGPUComputeResources
{
public:

	FAtomsXpuExampleJointTransformXPUComputeResources();

	virtual ~FAtomsXpuExampleJointTransformXPUComputeResources();

	virtual void ReleaseResources() override;

	FAgentBonesBufferAndSRV Buffer;
	static const int32 Stride;
};

/* Compute Data
* This object is reused at each tick. Please be aware there could be multiple of this object active.
* At each tick one of this object is requested to the agent type component and it is filled and sent to the update render data queue.
* The render data queue then send this and the dispatcher to the render thread in order to update the buffer and put the compute shader inside the queue
* of the RDGGraph build by the agent type component. After this is done the compute data is sent to the agent type component and saved on a list of used objects that can be used in other ticks.
* Since the render thread has 2-3 frames of delay there could be 2 or 3 of these objects live at the same time.
*/
class AtomsXpuExampleJointTransformXPUComputeData : public AgentPoseGPUComputeData
{
public:

	static int32 staticTypeId;

	AtomsXpuExampleJointTransformXPUComputeData();

	virtual ~AtomsXpuExampleJointTransformXPUComputeData();

	virtual int32 typeId() override;

	FTransform ActorTransform;
	FString AgentTypeName;
	TArray<float> Data;
};

/* This is a dispatcher rsponsibles to upload buffers and execute the compute shader
*
*/
class FAtomsXpuExampleJointTransformXPUCompute : public FAgentPoseGPUCompute
{
public:

	static int32 staticTypeId;

	virtual int32 typeId() const override;

	FAtomsXpuExampleJointTransformXPUCompute();

	virtual ~FAtomsXpuExampleJointTransformXPUCompute();

	/* This is called in game thread before the FAtomsAgentTypeRenderObject enqueue the works for the render thread
	*	inside the update render data method. You can safely access here the game data.
	*/
	virtual void Update(FAtomsAgentTypeRenderObject* RenderObject, FAgentTypePoseRenderData* BonesData, TSharedPtr<AgentPoseGPUComputeData, ESPMode::ThreadSafe>& ComputeData) override;

	// The execute is called inside the render thread after the poses buffer is already allocated
	virtual void Execute(FRHICommandListImmediate& RhiCmdList, FRDGBuilder& GraphBuilder, FAgentTypeRDGBuffers& AgentTypeBuffers, FAtomsAgentTypeRenderObject* RenderObject, FAgentTypePoseRenderData* BonesData, TSharedPtr<AgentPoseGPUComputeData, ESPMode::ThreadSafe>& ComputeData) override;

	// Called by the FAtomsAgentTypeRenderObject before releasing its own render data
	virtual void ReleaseResources(FAtomsAgentTypeRenderObject* RenderObject) override;

	// Called by the FAtomsAgentTypeRenderObject after it initialize its render data
	virtual void InitResources(FAtomsAgentTypeRenderObject* RenderObject) override;

	FCriticalSection Mutex;
	FAgentPoseGPUComputeResources* PoseGPUComputeResources;
	TWeakObjectPtr<UAtomsXpuExampleJointTransform_BehaviourComponent> Component;
	FTransform ActorTransform;
};


/* This is the compute dispatcher for the behaviour tree task node.
* It is similar to the normal compute dispatcher above, the only difference is how it pass the parameter to the compute shader.
* In the standard dispatcher the parameter ar passed by simple values since all the agents uses those values.
* Inside the behavioru tree not all the agents could run this at the same time, so this dispatcher send the parameters per agent
*/
class FAtomsXpuExampleJointTransformXPUDispatcher : public FAtomsBehaviourTreeXPUDispatcher
{
public:

	static int32 staticTypeId;

	virtual int32 typeId() const override;

	FAtomsXpuExampleJointTransformXPUDispatcher();

	virtual ~FAtomsXpuExampleJointTransformXPUDispatcher();

	/* This is called in game thread before the FAtomsAgentTypeRenderObject enqueue the works for the render thread
	*	inside the update render date method. You can safely access here the game data.
	*/
	virtual void Update(FAtomsAgentTypeRenderObject* RenderObject, FAgentTypePoseRenderData* BonesData, TSharedPtr<AgentPoseGPUComputeData, ESPMode::ThreadSafe>& ComputeData) override;

	// The execute is called inside the render thread after the poses buffer is already allocated
	virtual void Execute(FRHICommandListImmediate& RhiCmdList, FRDGBuilder& GraphBuilder, FAgentTypeRDGBuffers& AgentTypeBuffers, FAtomsAgentTypeRenderObject* RenderObject, FAgentTypePoseRenderData* BonesData, TSharedPtr<AgentPoseGPUComputeData, ESPMode::ThreadSafe>& ComputeData) override;

	// Called by the FAtomsAgentTypeRenderObject before releasing its own render data
	virtual void ReleaseResources(FAtomsAgentTypeRenderObject* RenderObject) override;

	// Called by the FAtomsAgentTypeRenderObject after it initialize its render data
	virtual void InitResources(FAtomsAgentTypeRenderObject* RenderObject) override;

	virtual void UpdateData(UBehaviourTree_BehaviourComponent::NodeGpuData& NodeData) override;

	FCriticalSection Mutex;
	FBehaviourTreeXPUComputeResources* PoseGPUComputeResources;
};

