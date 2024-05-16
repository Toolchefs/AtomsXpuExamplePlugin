// ===========================================================================
// Copyright (c) 2015 Toolchefs Ltd. All rights reserved.
//
// Use of this software is subject to the terms of the Toolchefs license
// agreement provided at the time of installation or download, or which
// otherwise accompanies this software in either electronic or hard copy form.
// ===========================================================================
#pragma once

#include "CoreMinimal.h"
#include "Components/AtomsBehaviourComponent.h"
#include <Atoms/BehaviourModule.h>
#include "Rendering/SkeletalMeshInstancedFactory.h"
#include "AtomsXpuExampleJointTransformComponent.generated.h"

class FAgentPoseGPUCompute;
class AgentPoseGPUComputeData;

UCLASS(ClassGroup = (AtomsBehaviours), meta = (BlueprintSpawnableComponent))
class ATOMSXPUEXAMPLE_API UAtomsXpuExampleJointTransform_BehaviourComponent : public UAtomsBehaviourComponent
{
	GENERATED_BODY()

public:

	/**The joint name */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Interp, Category = "Behaviour", meta = (AtomsType = "9908", AtomsName = "jointName"))
	FName jointName;

	/** The joint name */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Interp, Category = "Behaviour", meta = (AtomsType = "9908", AtomsName = "jointNameOverride", AtomsOverride = "jointName"))
	TMap<int32, FName> jointNameOverride;

	/** Apply the transformation as an offset to the current one */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Interp, Category = "Behaviour", meta = (AtomsType = "13", AtomsName = "offset"))
	bool offset;

	/** Apply the transformation as an offset to the current one */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Interp, Category = "Behaviour", meta = (AtomsType = "1", AtomsName = "offsetOverride", AtomsOverride = "offset"))
	TMap<int32, bool> offsetOverride;

	/** Translation value */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Interp, Category = "Behaviour", meta = (AtomsType = "5", AtomsName = "translate"))
	FVector translate;

	/** Translation value */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Interp, Category = "Behaviour", meta = (AtomsType = "1", AtomsName = "translateOverride", AtomsOverride = "translate"))
	TMap<int32, FVector> translateOverride;

	/** Rotation value in degrees */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Interp, Category = "Behaviour", meta = (AtomsType = "5", AtomsName = "rotate"))
	FVector rotate;

	/** Rotation value in degrees */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Interp, Category = "Behaviour", meta = (AtomsType = "1", AtomsName = "rotateOverride", AtomsOverride = "rotate"))
	TMap<int32, FVector> rotateOverride;

	/** Scale value */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Interp, Category = "Behaviour", meta = (AtomsType = "5", AtomsName = "scale"))
	FVector scale;

	/** Scale value */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Interp, Category = "Behaviour", meta = (AtomsType = "1", AtomsName = "scaleOverride", AtomsOverride = "scale"))
	TMap<int32, FVector> scaleOverride;

	/** The rotation order */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Interp, Category = "Behaviour", meta = (AtomsType = "3", AtomsName = "rotationOrder"))
	int32 rotationOrder;

	/** The rotation order */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Interp, Category = "Behaviour", meta = (AtomsType = "1", AtomsName = "rotationOrderOverride", AtomsOverride = "rotationOrder"))
	TMap<int32, int32> rotationOrderOverride;

	/** Apply the transformation in world space */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Interp, Category = "Behaviour", meta = (AtomsType = "13", AtomsName = "worldSpace"))
	bool worldSpace;

	/** Apply the transformation in world space */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Interp, Category = "Behaviour", meta = (AtomsType = "1", AtomsName = "worldSpaceOverride", AtomsOverride = "worldSpace"))
	TMap<int32, bool> worldSpaceOverride;

	/** If true, the transformation will be applied by an operator instead of the module itself at the endFrame stage.
	This option should be enabled when a joint transform module is used on an agent group using other modules
	actively modifying the pose with an operator and not at the endFrame stage(i.e.Sync) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Interp, Category = "Behaviour", meta = (AtomsType = "13", AtomsName = "useOperator"))
	bool useOperator;

	/** If true, the transformation will be applied by an operator instead of the module itself at the endFrame stage.
	This option should be enabled when a joint transform module is used on an agent group using other modules
	actively modifying the pose with an operator and not at the endFrame stage(i.e.Sync) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Interp, Category = "Behaviour", meta = (AtomsType = "1", AtomsName = "useOperatorOverride", AtomsOverride = "useOperator"))
	TMap<int32, bool> useOperatorOverride;

	/** The weight */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Interp, Category = "Behaviour", meta = (AtomsType = "4", AtomsName = "weight"), meta = (UIMin = "0.00", UIMax = "1.00"))
	float weight;

	/** The weight */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Interp, Category = "Behaviour", meta = (AtomsType = "1", AtomsName = "weightOverride", AtomsOverride = "weight"))
	TMap<int32, float> weightOverride;

	/** The joint name */
	UFUNCTION(BlueprintCallable, Category = "JointTransformBehaviourComponent", meta = (CallInEditor = "true"))
	void SetJointName(const FName& Value);

	/** The joint name */
	UFUNCTION(BlueprintCallable, Category = "JointTransformBehaviourComponent", meta = (CallInEditor = "true"))
	void SetJointNameOverride(const FName& Value, const int32 AgentId);

	/** Remove agent override */
	UFUNCTION(BlueprintCallable, Category = "JointTransformBehaviourComponent", meta = (CallInEditor = "true"))
	void RemoveJointNameOverride(const int32 AgentId);

	/** Apply the transformation as an offset to the current one */
	UFUNCTION(BlueprintCallable, Category = "JointTransformBehaviourComponent", meta = (CallInEditor = "true"))
	void SetOffset(const bool Value);

	/** Apply the transformation as an offset to the current one */
	UFUNCTION(BlueprintCallable, Category = "JointTransformBehaviourComponent", meta = (CallInEditor = "true"))
	void SetOffsetOverride(const bool Value, const int32 AgentId);

	/** Remove agent override */
	UFUNCTION(BlueprintCallable, Category = "JointTransformBehaviourComponent", meta = (CallInEditor = "true"))
	void RemoveOffsetOverride(const int32 AgentId);

	/** Translation value */
	UFUNCTION(BlueprintCallable, Category = "JointTransformBehaviourComponent", meta = (CallInEditor = "true"))
	void SetTranslate(const FVector& Value);

	/** Translation value */
	UFUNCTION(BlueprintCallable, Category = "JointTransformBehaviourComponent", meta = (CallInEditor = "true"))
	void SetTranslateOverride(const FVector& Value, const int32 AgentId);

	/** Remove agent override */
	UFUNCTION(BlueprintCallable, Category = "JointTransformBehaviourComponent", meta = (CallInEditor = "true"))
	void RemoveTranslateOverride(const int32 AgentId);

	/** Rotation value in degrees */
	UFUNCTION(BlueprintCallable, Category = "JointTransformBehaviourComponent", meta = (CallInEditor = "true"))
	void SetRotate(const FVector& Value);

	/** Rotation value in degrees */
	UFUNCTION(BlueprintCallable, Category = "JointTransformBehaviourComponent", meta = (CallInEditor = "true"))
	void SetRotateOverride(const FVector& Value, const int32 AgentId);

	/** Remove agent override */
	UFUNCTION(BlueprintCallable, Category = "JointTransformBehaviourComponent", meta = (CallInEditor = "true"))
	void RemoveRotateOverride(const int32 AgentId);

	/** Scale value */
	UFUNCTION(BlueprintCallable, Category = "JointTransformBehaviourComponent", meta = (CallInEditor = "true"))
	void SetScale(const FVector& Value);

	/** Scale value */
	UFUNCTION(BlueprintCallable, Category = "JointTransformBehaviourComponent", meta = (CallInEditor = "true"))
	void SetScaleOverride(const FVector& Value, const int32 AgentId);

	/** Remove agent override */
	UFUNCTION(BlueprintCallable, Category = "JointTransformBehaviourComponent", meta = (CallInEditor = "true"))
	void RemoveScaleOverride(const int32 AgentId);

	/** The rotation order */
	UFUNCTION(BlueprintCallable, Category = "JointTransformBehaviourComponent", meta = (CallInEditor = "true"))
	void SetRotationOrder(const int32 Value);

	/** The rotation order */
	UFUNCTION(BlueprintCallable, Category = "JointTransformBehaviourComponent", meta = (CallInEditor = "true"))
	void SetRotationOrderOverride(const int32 Value, const int32 AgentId);

	/** Remove agent override */
	UFUNCTION(BlueprintCallable, Category = "JointTransformBehaviourComponent", meta = (CallInEditor = "true"))
	void RemoveRotationOrderOverride(const int32 AgentId);

	/** Apply the transformation in world space */
	UFUNCTION(BlueprintCallable, Category = "JointTransformBehaviourComponent", meta = (CallInEditor = "true"))
	void SetWorldSpace(const bool Value);

	/** Apply the transformation in world space */
	UFUNCTION(BlueprintCallable, Category = "JointTransformBehaviourComponent", meta = (CallInEditor = "true"))
	void SetWorldSpaceOverride(const bool Value, const int32 AgentId);

	/** Remove agent override */
	UFUNCTION(BlueprintCallable, Category = "JointTransformBehaviourComponent", meta = (CallInEditor = "true"))
	void RemoveWorldSpaceOverride(const int32 AgentId);

	/** If true, the transformation will be applied by an operator instead of the module itself at the endFrame stage.
This option should be enabled when a joint transform module is used on an agent group using other modules
actively modifying the pose with an operator and not at the endFrame stage(i.e.Sync) */
	UFUNCTION(BlueprintCallable, Category = "JointTransformBehaviourComponent", meta = (CallInEditor = "true"))
	void SetUseOperator(const bool Value);

	/** If true, the transformation will be applied by an operator instead of the module itself at the endFrame stage.
This option should be enabled when a joint transform module is used on an agent group using other modules
actively modifying the pose with an operator and not at the endFrame stage(i.e.Sync) */
	UFUNCTION(BlueprintCallable, Category = "JointTransformBehaviourComponent", meta = (CallInEditor = "true"))
	void SetUseOperatorOverride(const bool Value, const int32 AgentId);

	/** Remove agent override */
	UFUNCTION(BlueprintCallable, Category = "JointTransformBehaviourComponent", meta = (CallInEditor = "true"))
	void RemoveUseOperatorOverride(const int32 AgentId);

	/** The weight */
	UFUNCTION(BlueprintCallable, Category = "JointTransformBehaviourComponent", meta = (CallInEditor = "true"))
	void SetWeight(const float Value);

	/** The weight */
	UFUNCTION(BlueprintCallable, Category = "JointTransformBehaviourComponent", meta = (CallInEditor = "true"))
	void SetWeightOverride(const float Value, const int32 AgentId);

	/** Remove agent override */
	UFUNCTION(BlueprintCallable, Category = "JointTransformBehaviourComponent", meta = (CallInEditor = "true"))
	void RemoveWeightOverride(const int32 AgentId);

	AtomsCore::Euler::Order convertRotateOrderToEulerRot(int value);

	UAtomsXpuExampleJointTransform_BehaviourComponent();

	~UAtomsXpuExampleJointTransform_BehaviourComponent();

	void OnBehaviourModuleCreated(AtomsPtr<Atoms::BehaviourModule>& module) override;

	virtual void GetAtomsAttributes(AtomsPtr<AtomsCore::MapMetadata>& attributes) override;

	virtual void SetAtomsAttributes(std::shared_ptr<AtomsCore::MapMetadata>& attributes, std::shared_ptr<AtomsCore::MapMetadata>& hostData, FAtomsSceneAssets& sceneAssets) override;

	virtual bool IsUsingGPUComputePose() const override;

	virtual void UpdateGpuComputeData(class UAtomsAgentTypeComponent* AgentTypeComponent, struct FAgentTypePoseRenderData* AgentTypePoseRenderData, uint32 Index, uint32& NumActivePerInstanceMetadata) override;

	static void RegisterBehaviourModule();

	static void UnRegisterBehaviourModule();

private:

	FCriticalSection Mutex;
};


class ATOMSXPUEXAMPLE_API AtomsXpuExampleJointTransformModule : public Atoms::BehaviourModule
{
public:

	AtomsXpuExampleJointTransformModule();

	virtual ~AtomsXpuExampleJointTransformModule();

	static Atoms::BehaviourModule* creator(const std::string& parameter);

	void setComponent(UAtomsXpuExampleJointTransform_BehaviourComponent* component) { m_component = component; }

private:

	TWeakObjectPtr<UAtomsXpuExampleJointTransform_BehaviourComponent> m_component;

	double m_simTime;
};