// ===========================================================================
// Copyright (c) 2015 Toolchefs Ltd. All rights reserved.
//
// Use of this software is subject to the terms of the Toolchefs license
// agreement provided at the time of installation or download, or which
// otherwise accompanies this software in either electronic or hard copy form.
// ===========================================================================
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "BehaviourTree/AtomsBehaviourTreeTaskNode.h"
#include "BehaviourTree/AtomsBehaviourTreeNodeProperty.h"
#include "BehaviourTree/AtomsBehaviourTreeXPUNode.h"
#include "AtomsXpuExampleJointTransformTaskNode.generated.h"

UCLASS(ClassGroup = (AtomsBehavioursTreeNode))
class ATOMSXPUEXAMPLE_API UAtomsXpuExampleJointTransformTaskNode : public UAtomsBehaviourTreeTaskNode
{
	GENERATED_UCLASS_BODY()

#if WITH_EDITOR
		virtual FName GetNodeIconName() const override;
#endif

	virtual FString GetAtomsClass() const override;

	virtual void GetAtomsAttribute(AtomsCore::MapMetadata* attributes) const override;

	virtual void SetAtomsAttribute(AtomsCore::MapMetadata* attributes) override;

	virtual FString GetStaticDescription() const override;

	virtual Atoms::Behaviour* GenerateAtomsBehaviour(AActor* Actor, UActorComponent* Component) override;

public:

	/** Joint name */
	UPROPERTY(EditAnywhere, Category = "Properties")
		FAtomsBehaviourTreeNodeStringProperty jointName;

	/** If on, ensures no data on the agent is written simultaneously by nodes that have this option active. */
	UPROPERTY(EditAnywhere, Category = "Properties")
		FAtomsBehaviourTreeNodeBoolProperty lockAgent;

	/** Mode:<br/>0: As last operator<br/>1: Pre-physic operator<br/>2: Pre-pelvis operator */
	UPROPERTY(EditAnywhere, Category = "Properties")
		FAtomsBehaviourTreeNodeIntProperty mode;

	/** Apply the transformation as an offset */
	UPROPERTY(EditAnywhere, Category = "Properties")
		FAtomsBehaviourTreeNodeBoolProperty offset;

	/** Rotation */
	UPROPERTY(EditAnywhere, Category = "Properties")
		FAtomsBehaviourTreeNodeVector3Property rotation;

	/** Rotation order:<br/>0: XYZ<br/>1: YZX<br/>2: ZXY<br/>3: XZY<br/>4: YXZ<br/>5: ZYX */
	UPROPERTY(EditAnywhere, Category = "Properties")
		FAtomsBehaviourTreeNodeIntProperty rotationOrder;

	/** Scale */
	UPROPERTY(EditAnywhere, Category = "Properties")
		FAtomsBehaviourTreeNodeVector3Property scale;

	/** Translation */
	UPROPERTY(EditAnywhere, Category = "Properties")
		FAtomsBehaviourTreeNodeVector3Property translation;

	/** Blending weight */
	UPROPERTY(EditAnywhere, Category = "Properties")
		FAtomsBehaviourTreeNodeDoubleProperty weight;

	/** Apply the transformation in world space */
	UPROPERTY(EditAnywhere, Category = "Properties")
		FAtomsBehaviourTreeNodeBoolProperty worldSpace;

	/** If active allocate for each agent the data for the gpu task.
		* Activate this only if you have a lot of agents executing this node a t the same time. */
	UPROPERTY(EditAnywhere, Category = "Properties")
	bool preallocateMemory;
};


class AtomsXpuExampleJointTransformTaskNodeWrapper : public AtomsBehaviourTreeXPUNode
{
public:

	AtomsXpuExampleJointTransformTaskNodeWrapper();

	virtual ~AtomsXpuExampleJointTransformTaskNodeWrapper();

	virtual const char* typeName() const override;

	static const char* staticTypeName();

	virtual void initialize(Atoms::BehaviourTreeContext* context, Atoms::Behaviour::State* data) override;

	virtual void update(Atoms::BehaviourTreeContext* context, Atoms::Behaviour::State* data) override;

	virtual void terminate(Atoms::BehaviourTreeContext* context, Atoms::Behaviour::State* data) override;

	virtual void releaseData(Atoms::BehaviourTreeContext* context, Atoms::Behaviour::State* data) override;

	virtual void setAttributes(const AtomsCore::MapMetadata* attributes, Atoms::Blackboard* blackboard) override;

	virtual void getAttributes(AtomsCore::MapMetadata* attributes, Atoms::Blackboard* blackboard) override;

	virtual void getAttributeProperties(AtomsCore::MapMetadata* attributes) override;

	static Atoms::Behaviour* creator();

	virtual size_t GpuDataStride() const override;

	virtual TSharedPtr<FAtomsBehaviourTreeXPUDispatcher, ESPMode::ThreadSafe> CreateDispatcher() override;

	AtomsCore::Euler::Order convertRotateOrderToEulerRot(int value);

public:

	Atoms::BlackboardStringValue jointName;

	Atoms::BlackboardVector3Value translation;

	Atoms::BlackboardVector3Value rotation;

	Atoms::BlackboardVector3Value scale;

	Atoms::BlackboardDoubleValue weight;

	Atoms::BlackboardIntValue rotationOrder;

	Atoms::BlackboardIntValue mode;

	Atoms::BlackboardBoolValue offset;

	Atoms::BlackboardBoolValue worldSpace;

	TWeakObjectPtr<class AAtomsAgentGroup> AgentGroup;
	TWeakObjectPtr<class UBehaviourTree_BehaviourComponent> Component;
};