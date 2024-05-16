#include "AtomsXpuExampleJointTransformComponent.h"

#include "Components/AtomsAgentTypeComponent.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "RenderGraphResources.h"
#include "Rendering/AtomsComputeShaders.h"
#include "XPUCompute/XPUComputeUtils.h"
#include "AtomsXpuExampleDispatcher.h"
#include "Actors/AtomsAgentGroup.h"
#include "AtomsModuleGeneratorUtils.h"
#include "AtomsAttributeUtils.h"
#include <Atoms/AgentGroup.h>
#include <AtomsCore/Metadata/IntMetadata.h>
#include <AtomsCore/Metadata/BoolMetadata.h>
#include <AtomsUtils/TaskScheduler.h>
#include <Atoms/BehaviourModules.h>

static const char* XpuExampleJointTransformXPUModuleName = "AtomsXpuExampleJointTransform";

void UAtomsXpuExampleJointTransform_BehaviourComponent::RegisterBehaviourModule()
{
	Atoms::BehaviourModules::instance().registerBehaviourModule(XpuExampleJointTransformXPUModuleName, &AtomsXpuExampleJointTransformModule::creator, Atoms::BehaviourModules::kNative, true, "Unreal");
}

void UAtomsXpuExampleJointTransform_BehaviourComponent::UnRegisterBehaviourModule()
{
	Atoms::BehaviourModules::instance().deregisterBehaviourModule(XpuExampleJointTransformXPUModuleName);
}



UAtomsXpuExampleJointTransform_BehaviourComponent::UAtomsXpuExampleJointTransform_BehaviourComponent()
{
	AtomsBehaviourModule = XpuExampleJointTransformXPUModuleName;
	jointName = "";
	offset = true;
	translate = FVector(0.0, 0.0, 0.0);
	rotate = FVector(0.0, 0.0, 0.0);
	scale = FVector(1.0, 1.0, 1.0);
	rotationOrder = 0;
	worldSpace = false;
	useOperator = false;
	weight = 1.0;
}

UAtomsXpuExampleJointTransform_BehaviourComponent::~UAtomsXpuExampleJointTransform_BehaviourComponent()
{

}

void UAtomsXpuExampleJointTransform_BehaviourComponent::OnBehaviourModuleCreated(AtomsPtr<Atoms::BehaviourModule>& module)
{
	if (!module)
		return;

	auto Actor = GetAgentGroup();
	if (!Actor)
		return;

	auto World = Actor->GetWorld();
	if (!World)
		return;

	if (module->typeName() != XpuExampleJointTransformXPUModuleName)
		return;

	auto skelmodule = std::static_pointer_cast<AtomsXpuExampleJointTransformModule>(module);
	if (!skelmodule)
		return;

	skelmodule->setComponent(this);
}

void UAtomsXpuExampleJointTransform_BehaviourComponent::GetAtomsAttributes(AtomsPtr<AtomsCore::MapMetadata>& attributes)
{
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_META(jointName, AtomsCore::StringMetadata, UAtomsAttributeUtils::ToString);
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_META_OVERRIDE(jointName_override, jointNameOverride, AtomsCore::StringMetadata, UAtomsAttributeUtils::ToString);
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_META(offset, AtomsCore::BoolMetadata, );
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_META_OVERRIDE(offset_override, offsetOverride, AtomsCore::BoolMetadata, );
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_META(translate, AtomsCore::Vector3Metadata, AtomsConverter::ToVector3);
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_META_OVERRIDE(translate_override, translateOverride, AtomsCore::Vector3Metadata, AtomsConverter::ToVector3);
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_META(rotate, AtomsCore::Vector3Metadata, AtomsConverter::ToVector3);
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_META_OVERRIDE(rotate_override, rotateOverride, AtomsCore::Vector3Metadata, AtomsConverter::ToVector3);
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_META(scale, AtomsCore::Vector3Metadata, AtomsConverter::ToVector3);
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_META_OVERRIDE(scale_override, scaleOverride, AtomsCore::Vector3Metadata, AtomsConverter::ToVector3);
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_META(rotationOrder, AtomsCore::IntMetadata, );
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_META_OVERRIDE(rotationOrder_override, rotationOrderOverride, AtomsCore::IntMetadata, );
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_META(worldSpace, AtomsCore::BoolMetadata, );
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_META_OVERRIDE(worldSpace_override, worldSpaceOverride, AtomsCore::BoolMetadata, );
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_META(useOperator, AtomsCore::BoolMetadata, );
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_META_OVERRIDE(useOperator_override, useOperatorOverride, AtomsCore::BoolMetadata, );
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_META(weight, AtomsCore::DoubleMetadata, );
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_META_OVERRIDE(weight_override, weightOverride, AtomsCore::DoubleMetadata, );
}

void UAtomsXpuExampleJointTransform_BehaviourComponent::SetAtomsAttributes(std::shared_ptr<AtomsCore::MapMetadata>& attributes, std::shared_ptr<AtomsCore::MapMetadata>& hostData, FAtomsSceneAssets& sceneAssets)
{
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_PROPERTY_META(jointName, AtomsCore::StringMetadata, AtomsConverter::FromName);
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_PROPERTY_META_OVERRIDE(jointName_override, jointNameOverride, AtomsCore::StringMetadata, AtomsConverter::FromName);
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_PROPERTY_META(offset, AtomsCore::BoolMetadata, );
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_PROPERTY_META_OVERRIDE(offset_override, offsetOverride, AtomsCore::BoolMetadata, );
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_PROPERTY_META(translate, AtomsCore::Vector3Metadata, AtomsConverter::FromVector3);
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_PROPERTY_META_OVERRIDE(translate_override, translateOverride, AtomsCore::Vector3Metadata, AtomsConverter::FromVector3);
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_PROPERTY_META(rotate, AtomsCore::Vector3Metadata, AtomsConverter::FromVector3);
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_PROPERTY_META_OVERRIDE(rotate_override, rotateOverride, AtomsCore::Vector3Metadata, AtomsConverter::FromVector3);
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_PROPERTY_META(scale, AtomsCore::Vector3Metadata, AtomsConverter::FromVector3);
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_PROPERTY_META_OVERRIDE(scale_override, scaleOverride, AtomsCore::Vector3Metadata, AtomsConverter::FromVector3);
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_PROPERTY_META(rotationOrder, AtomsCore::IntMetadata, );
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_PROPERTY_META_OVERRIDE(rotationOrder_override, rotationOrderOverride, AtomsCore::IntMetadata, );
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_PROPERTY_META(worldSpace, AtomsCore::BoolMetadata, );
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_PROPERTY_META_OVERRIDE(worldSpace_override, worldSpaceOverride, AtomsCore::BoolMetadata, );
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_PROPERTY_META(useOperator, AtomsCore::BoolMetadata, );
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_PROPERTY_META_OVERRIDE(useOperator_override, useOperatorOverride, AtomsCore::BoolMetadata, );
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_PROPERTY_META(weight, AtomsCore::DoubleMetadata, );
	ATOMS_BEHAVIOUR_COMPONENT_CONVERT_PROPERTY_META_OVERRIDE(weight_override, weightOverride, AtomsCore::DoubleMetadata, );
}

void UAtomsXpuExampleJointTransform_BehaviourComponent::SetJointName(const FName& Value)
{
	jointName = Value;
	ATOMS_BEHAVIOUR_COMPONENT_GET_ATTRIBUTES()
		ATOMS_BEHAVIOUR_COMPONENT_CONVERT_META(jointName, AtomsCore::StringMetadata, UAtomsAttributeUtils::ToString);

}

void UAtomsXpuExampleJointTransform_BehaviourComponent::SetJointNameOverride(const FName& Value, const int32 AgentId)
{
	ATOMS_BEHAVIOUR_COMPONENT_SET_META_OVERRIDE(jointName_override, jointNameOverride, AtomsCore::StringMetadata, UAtomsAttributeUtils::ToString, AgentId);

}

void UAtomsXpuExampleJointTransform_BehaviourComponent::RemoveJointNameOverride(const int32 AgentId)
{
	ATOMS_BEHAVIOUR_COMPONENT_REMOVE_OVERRIDE(jointName_override, jointNameOverride, AgentId);
}

void UAtomsXpuExampleJointTransform_BehaviourComponent::SetOffset(const bool Value)
{
	offset = Value;
	ATOMS_BEHAVIOUR_COMPONENT_GET_ATTRIBUTES()
		ATOMS_BEHAVIOUR_COMPONENT_CONVERT_META(offset, AtomsCore::BoolMetadata, );

}

void UAtomsXpuExampleJointTransform_BehaviourComponent::SetOffsetOverride(const bool Value, const int32 AgentId)
{
	ATOMS_BEHAVIOUR_COMPONENT_SET_META_OVERRIDE(offset_override, offsetOverride, AtomsCore::BoolMetadata, , AgentId);

}

void UAtomsXpuExampleJointTransform_BehaviourComponent::RemoveOffsetOverride(const int32 AgentId)
{
	ATOMS_BEHAVIOUR_COMPONENT_REMOVE_OVERRIDE(offset_override, offsetOverride, AgentId);
}

void UAtomsXpuExampleJointTransform_BehaviourComponent::SetTranslate(const FVector& Value)
{
	translate = Value;
	ATOMS_BEHAVIOUR_COMPONENT_GET_ATTRIBUTES()
		ATOMS_BEHAVIOUR_COMPONENT_CONVERT_META(translate, AtomsCore::Vector3Metadata, AtomsConverter::ToVector3);

}

void UAtomsXpuExampleJointTransform_BehaviourComponent::SetTranslateOverride(const FVector& Value, const int32 AgentId)
{
	ATOMS_BEHAVIOUR_COMPONENT_SET_META_OVERRIDE(translate_override, translateOverride, AtomsCore::Vector3Metadata, AtomsConverter::ToVector3, AgentId);

}

void UAtomsXpuExampleJointTransform_BehaviourComponent::RemoveTranslateOverride(const int32 AgentId)
{
	ATOMS_BEHAVIOUR_COMPONENT_REMOVE_OVERRIDE(translate_override, translateOverride, AgentId);
}

void UAtomsXpuExampleJointTransform_BehaviourComponent::SetRotate(const FVector& Value)
{
	rotate = Value;
	ATOMS_BEHAVIOUR_COMPONENT_GET_ATTRIBUTES()
		ATOMS_BEHAVIOUR_COMPONENT_CONVERT_META(rotate, AtomsCore::Vector3Metadata, AtomsConverter::ToVector3);

}

void UAtomsXpuExampleJointTransform_BehaviourComponent::SetRotateOverride(const FVector& Value, const int32 AgentId)
{
	ATOMS_BEHAVIOUR_COMPONENT_SET_META_OVERRIDE(rotate_override, rotateOverride, AtomsCore::Vector3Metadata, AtomsConverter::ToVector3, AgentId);

}

void UAtomsXpuExampleJointTransform_BehaviourComponent::RemoveRotateOverride(const int32 AgentId)
{
	ATOMS_BEHAVIOUR_COMPONENT_REMOVE_OVERRIDE(rotate_override, rotateOverride, AgentId);
}

void UAtomsXpuExampleJointTransform_BehaviourComponent::SetScale(const FVector& Value)
{
	scale = Value;
	ATOMS_BEHAVIOUR_COMPONENT_GET_ATTRIBUTES()
		ATOMS_BEHAVIOUR_COMPONENT_CONVERT_META(scale, AtomsCore::Vector3Metadata, AtomsConverter::ToVector3);

}

void UAtomsXpuExampleJointTransform_BehaviourComponent::SetScaleOverride(const FVector& Value, const int32 AgentId)
{
	ATOMS_BEHAVIOUR_COMPONENT_SET_META_OVERRIDE(scale_override, scaleOverride, AtomsCore::Vector3Metadata, AtomsConverter::ToVector3, AgentId);

}

void UAtomsXpuExampleJointTransform_BehaviourComponent::RemoveScaleOverride(const int32 AgentId)
{
	ATOMS_BEHAVIOUR_COMPONENT_REMOVE_OVERRIDE(scale_override, scaleOverride, AgentId);
}

void UAtomsXpuExampleJointTransform_BehaviourComponent::SetRotationOrder(const int32 Value)
{
	rotationOrder = Value;
	ATOMS_BEHAVIOUR_COMPONENT_GET_ATTRIBUTES()
		ATOMS_BEHAVIOUR_COMPONENT_CONVERT_META(rotationOrder, AtomsCore::IntMetadata, );

}

void UAtomsXpuExampleJointTransform_BehaviourComponent::SetRotationOrderOverride(const int32 Value, const int32 AgentId)
{
	ATOMS_BEHAVIOUR_COMPONENT_SET_META_OVERRIDE(rotationOrder_override, rotationOrderOverride, AtomsCore::IntMetadata, , AgentId);

}

void UAtomsXpuExampleJointTransform_BehaviourComponent::RemoveRotationOrderOverride(const int32 AgentId)
{
	ATOMS_BEHAVIOUR_COMPONENT_REMOVE_OVERRIDE(rotationOrder_override, rotationOrderOverride, AgentId);
}

void UAtomsXpuExampleJointTransform_BehaviourComponent::SetWorldSpace(const bool Value)
{
	worldSpace = Value;
	ATOMS_BEHAVIOUR_COMPONENT_GET_ATTRIBUTES()
		ATOMS_BEHAVIOUR_COMPONENT_CONVERT_META(worldSpace, AtomsCore::BoolMetadata, );

}

void UAtomsXpuExampleJointTransform_BehaviourComponent::SetWorldSpaceOverride(const bool Value, const int32 AgentId)
{
	ATOMS_BEHAVIOUR_COMPONENT_SET_META_OVERRIDE(worldSpace_override, worldSpaceOverride, AtomsCore::BoolMetadata, , AgentId);

}

void UAtomsXpuExampleJointTransform_BehaviourComponent::RemoveWorldSpaceOverride(const int32 AgentId)
{
	ATOMS_BEHAVIOUR_COMPONENT_REMOVE_OVERRIDE(worldSpace_override, worldSpaceOverride, AgentId);
}

void UAtomsXpuExampleJointTransform_BehaviourComponent::SetUseOperator(const bool Value)
{
	useOperator = Value;
	ATOMS_BEHAVIOUR_COMPONENT_GET_ATTRIBUTES()
		ATOMS_BEHAVIOUR_COMPONENT_CONVERT_META(useOperator, AtomsCore::BoolMetadata, );

}

void UAtomsXpuExampleJointTransform_BehaviourComponent::SetUseOperatorOverride(const bool Value, const int32 AgentId)
{
	ATOMS_BEHAVIOUR_COMPONENT_SET_META_OVERRIDE(useOperator_override, useOperatorOverride, AtomsCore::BoolMetadata, , AgentId);

}

void UAtomsXpuExampleJointTransform_BehaviourComponent::RemoveUseOperatorOverride(const int32 AgentId)
{
	ATOMS_BEHAVIOUR_COMPONENT_REMOVE_OVERRIDE(useOperator_override, useOperatorOverride, AgentId);
}

void UAtomsXpuExampleJointTransform_BehaviourComponent::SetWeight(const float Value)
{
	weight = Value;
	ATOMS_BEHAVIOUR_COMPONENT_GET_ATTRIBUTES()
		ATOMS_BEHAVIOUR_COMPONENT_CONVERT_META(weight, AtomsCore::DoubleMetadata, );

}

void UAtomsXpuExampleJointTransform_BehaviourComponent::SetWeightOverride(const float Value, const int32 AgentId)
{
	ATOMS_BEHAVIOUR_COMPONENT_SET_META_OVERRIDE(weight_override, weightOverride, AtomsCore::DoubleMetadata, , AgentId);

}

void UAtomsXpuExampleJointTransform_BehaviourComponent::RemoveWeightOverride(const int32 AgentId)
{
	ATOMS_BEHAVIOUR_COMPONENT_REMOVE_OVERRIDE(weight_override, weightOverride, AgentId);
}

AtomsCore::Euler::Order UAtomsXpuExampleJointTransform_BehaviourComponent::convertRotateOrderToEulerRot(int value)
{
	AtomsCore::Euler::Order returnOrder;

	switch (value)
	{
	case 0:
		returnOrder = AtomsCore::Euler::XYZ;
		break;
	case 1:
		returnOrder = AtomsCore::Euler::YZX;
		break;
	case 2:
		returnOrder = AtomsCore::Euler::ZXY;
		break;
	case 3:
		returnOrder = AtomsCore::Euler::XZY;
		break;
	case 4:
		returnOrder = AtomsCore::Euler::YXZ;
		break;
	case 5:
		returnOrder = AtomsCore::Euler::ZYX;
		break;
	default:
		returnOrder = AtomsCore::Euler::XYZ;
		break;
	}

	return returnOrder;
}




bool UAtomsXpuExampleJointTransform_BehaviourComponent::IsUsingGPUComputePose() const
{
	return true;
}

void UAtomsXpuExampleJointTransform_BehaviourComponent::UpdateGpuComputeData(
	UAtomsAgentTypeComponent* AgentTypeComponent, 
	FAgentTypePoseRenderData* AgentTypePoseRenderData, 
	uint32 Index,
	uint32& NumActivePerInstanceMetadata)
{
	if (!AgentTypeComponent || !AgentTypeComponent->AgentType)
		return;

	AAtomsAgentGroup* Actor = Cast<AAtomsAgentGroup>(GetOwner());
	if (!Actor || !Actor->AgentGroup)
		return;

	auto& atypeMapper = Actor->AgentGroup->agentTypeMapperRef();
	auto it = atypeMapper.find(TCHAR_TO_UTF8(*AgentTypeComponent->AgentType->AgentTypeName));
	if (it == atypeMapper.end())
		return;


	auto& agents = it->second;
	if (agents.size() == 0)
	{
		return;
	}


	auto ComputeData = AgentTypePoseRenderData->GetComputeData<AtomsXpuExampleJointTransformXPUComputeData>(Index);
	TArray<float>& Data = ComputeData->Data;
	Data.SetNumZeroed(agents.size() * FAtomsXpuExampleJointTransformXPUComputeResources::Stride);

	ComputeData->ActorTransform = GetOwner()->GetTransform();
	ComputeData->AgentTypeName = AgentTypeComponent->AgentType->AgentTypeName;

	AtomsUtils::parallel_for(AtomsUtils::ParallelForRange(0, agents.size()), [&](const AtomsUtils::ParallelForRange& r)
	{
		for (size_t i = r.begin(); i != r.end(); ++i)
		{
			int32 DataOffset = i * FAtomsXpuExampleJointTransformXPUComputeResources::Stride;
			FMemory::Memzero(&Data[DataOffset], sizeof(float) * FAtomsXpuExampleJointTransformXPUComputeResources::Stride);
			auto agent = agents[i];
			if (!agent)
				continue;
			
			auto JointId = agent->agentType()->skeleton().jointId(TCHAR_TO_UTF8(*jointName.ToString()));
			auto transOver = translateOverride.Find(agent->groupId()->get());
			FVector translationValue = transOver ? *transOver : translate;
			auto scaleOver = scaleOverride.Find(agent->groupId()->get());
			FVector scaleValue = scaleOver ? *scaleOver : scale;

			auto rotateOver = rotateOverride.Find(agent->groupId()->get());
			auto rotationValue = rotateOver ? *rotateOver : rotate;

			AtomsCore::Euler erotation(rotationValue.X * M_PI / 180.0, rotationValue.Z * M_PI / 180.0, rotationValue.Y * M_PI / 180.0, convertRotateOrderToEulerRot(rotationOrder));
			AtomsCore::Quaternion quatTmp = erotation.toQuat();

			Data[DataOffset + 0] = agent->groupId()->value();
			Data[DataOffset + 1] = JointId;
			Data[DataOffset + 2] = translationValue.X;
			Data[DataOffset + 3] = -translationValue.Z;
			Data[DataOffset + 4] = translationValue.Y;
			Data[DataOffset + 5] = quatTmp.v.x;
			Data[DataOffset + 6] = -quatTmp.v.y;
			Data[DataOffset + 7] = quatTmp.v.z;
			Data[DataOffset + 8] = -quatTmp.r;
			Data[DataOffset + 9] = scaleValue.X;
			Data[DataOffset + 10] = scaleValue.Y;
			Data[DataOffset + 11] = scaleValue.Z;
			Data[DataOffset + 12] = weight;
			Data[DataOffset + 13] = offset;
			Data[DataOffset + 14] = worldSpace;
			Data[DataOffset + 15] = 0;

		}
	});

	auto Dispatcher = AgentTypeComponent->GetGPUComputeDispatcher<FAtomsXpuExampleJointTransformXPUCompute>(Index);
	Dispatcher->Component = this;
	ComputeData->Dispatcher = Dispatcher;
	
}


AtomsXpuExampleJointTransformModule::AtomsXpuExampleJointTransformModule() : Atoms::BehaviourModule()
{
	AtomsCore::StringMetadata jointName;
	addAttribute("jointName", &jointName, true);
	AtomsCore::StringMetadata tooltipJointName("The joint name");
	addAttributeProperty("jointName", ATOMS_BEHAVIOUR_TOOLTIP, &tooltipJointName);
	AtomsCore::StringMetadata jointNameConstructor(Atoms::GlobalNameKeys::ATOMS_BEHAVIOUR_HARD_CONSTRUCTOR_JOINT_NAME_KEY);
	addAttributeProperty("jointName", Atoms::GlobalNameKeys::ATOMS_BEHAVIOUR_HARD_CONSTRUCTOR_KEY, &jointNameConstructor);

	AtomsCore::IntMetadata order(0);
	addAttribute("rotationOrder", &order, true);
	AtomsCore::StringMetadata tooltipOrder("The rotation order");
	addAttributeProperty("rotationOrder", ATOMS_BEHAVIOUR_TOOLTIP, &tooltipOrder);

	AtomsCore::BoolMetadata worldSpace(false);
	addAttribute("worldSpace", &worldSpace, true);
	AtomsCore::StringMetadata tooltipWorldSpace("Apply the transformation in world space");
	addAttributeProperty("worldSpace", ATOMS_BEHAVIOUR_TOOLTIP, &tooltipWorldSpace);

	AtomsCore::BoolMetadata add(true);
	addAttribute("offset", &add, true);
	AtomsCore::StringMetadata tooltipOffset("Apply the transformation as an offset to the current one");
	addAttributeProperty("offset", ATOMS_BEHAVIOUR_TOOLTIP, &tooltipOffset);

	AtomsCore::Vector3Metadata rotate(AtomsCore::Vector3(0.0, 0.0, 0.0));
	addAttribute("rotate", &rotate, true);
	AtomsCore::StringMetadata tooltipRotate("Rotation value in degrees");
	addAttributeProperty("rotate", ATOMS_BEHAVIOUR_TOOLTIP, &tooltipRotate);

	AtomsCore::Vector3Metadata scale(AtomsCore::Vector3(1.0, 1.0, 1.0));
	addAttribute("scale", &scale, true);
	AtomsCore::StringMetadata tooltipScale("Scale value");
	addAttributeProperty("scale", ATOMS_BEHAVIOUR_TOOLTIP, &tooltipScale);

	AtomsCore::Vector3Metadata translate(AtomsCore::Vector3(0.0, 0.0, 0.0));
	addAttribute("translate", &translate, true);
	AtomsCore::StringMetadata tooltipTranslate("Translation value");
	addAttributeProperty("translate", ATOMS_BEHAVIOUR_TOOLTIP, &tooltipTranslate);

	AtomsCore::DoubleMetadata weight(1.0);
	addAttribute("weight", &weight, true);
	AtomsCore::StringMetadata tooltipWeight("The weight");
	addAttributeProperty("weight", ATOMS_BEHAVIOUR_TOOLTIP, &tooltipWeight);
	AtomsCore::DoubleMetadata minWeight(0.0);
	addAttributeProperty("weight", ATOMS_BEHAVIOUR_MIN_VALUE, &minWeight);
	AtomsCore::DoubleMetadata maxWeight(1.0);
	addAttributeProperty("weight", ATOMS_BEHAVIOUR_MAX_VALUE, &maxWeight);

	AtomsCore::BoolMetadata useOperator(false);
	addAttribute("useOperator", &useOperator, true);
	AtomsCore::StringMetadata tooltipUseOperator(
		"If true, the transformation will be applied by an operator instead of the module itself at the endFrame stage.\n"
		"This option should be enabled when a joint transform module is used on an agent group using other modules\n"
		"actively modifying the pose with an operator and not at the endFrame stage(i.e.Sync)");
	addAttributeProperty("useOperator", ATOMS_BEHAVIOUR_TOOLTIP, &tooltipUseOperator);

	// Display order
	std::vector<std::string> displayOrderVector{
		"jointName", "offset", "translate", "rotate", "scale", "rotationOrder",
		"worldSpace", "useOperator", "weight" };
	AtomsCore::StringArrayMetadata displayOrder(displayOrderVector);
	addAttributeProperty(ATOMS_BEHAVIOUR_MODULE_PROPERTIES, ATOMS_BEHAVIOUR_MODULE_DISPLAY_ORDER, &displayOrder);
}

AtomsXpuExampleJointTransformModule::~AtomsXpuExampleJointTransformModule()
{

}

Atoms::BehaviourModule* AtomsXpuExampleJointTransformModule::creator(const std::string& parameter)
{
	return new AtomsXpuExampleJointTransformModule;
}
