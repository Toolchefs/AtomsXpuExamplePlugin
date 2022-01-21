#include "AtomsXpuExampleJointTransformTaskNode.h"
#include <Atoms/BehaviourTree/BehaviourTreeContext.h>
#include <Atoms/BehaviourTree/BehaviourTree.h>
#include <AtomsCore/Metadata/StringMetadata.h>
#include <AtomsCore/Metadata/IntMetadata.h>
#include <AtomsCore/Metadata/BoolMetadata.h>
#include <AtomsCore/Metadata/Metadata.h>
#include <AtomsCore/Metadata/StringArrayMetadata.h>
#include <Atoms/Graph/Operators/JointTransformOperator.h>
#include <Atoms/BehaviourTree/BehaviourTree.h>
#include <Atoms/BehaviourTree/BehaviourTreeContext.h>
#include <Atoms/BehaviourTree/Blackboard.h>
#include "AtomsConversions.h"
#include "Components/AtomsBehaviourTreeComponent.h"
#include "Actors/AtomsAgentGroup.h"
//#include "XPUCompute/BehaviourTree/ComputeData.h"

#define LOCTEXT_NAMESPACE "AtomsBehaviourTree"

UAtomsXpuExampleJointTransformTaskNode::UAtomsXpuExampleJointTransformTaskNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "AtomsXpuExampleJointTransform";


	jointName.Value = "";
	lockAgent.Value = false;
	mode.Value = 0;
	offset.Value = true;
	rotation.Value = FVector(0.0, 0.0, 0.0);
	rotationOrder.Value = 0;
	scale.Value = FVector(1.0, 1.0, 1.0);
	translation.Value = FVector(0.0, 0.0, 0.0);
	weight.Value = 1.0;
	worldSpace.Value = false;
}

FString UAtomsXpuExampleJointTransformTaskNode::GetStaticDescription() const
{
	return FString("Sets/Adds a transformation to a joint.\n\nSuccess: Never.\n\nFailure: When the transformation could not be applied.\n\nRunning: When the transformation is applied.");
}

#if WITH_EDITOR
FName UAtomsXpuExampleJointTransformTaskNode::GetNodeIconName() const
{
	return FName("AtomsIcons.BehaviourTree.Task.JointTransformXPU");
}

#endif // WITH_EDITOR

FString UAtomsXpuExampleJointTransformTaskNode::GetAtomsClass() const
{
	return FString("AtomsXpuExampleJointTransform");
}

void UAtomsXpuExampleJointTransformTaskNode::GetAtomsAttribute(AtomsCore::MapMetadata* attributes) const
{
	jointName.GetAtomsAttribute(FString("jointName"), attributes);
	lockAgent.GetAtomsAttribute(FString("lockAgent"), attributes);
	mode.GetAtomsAttribute(FString("mode"), attributes);
	offset.GetAtomsAttribute(FString("offset"), attributes);
	rotation.GetAtomsAttribute(FString("rotation"), attributes);
	rotationOrder.GetAtomsAttribute(FString("rotationOrder"), attributes);
	scale.GetAtomsAttribute(FString("scale"), attributes);
	translation.GetAtomsAttribute(FString("translation"), attributes);
	weight.GetAtomsAttribute(FString("weight"), attributes);
	worldSpace.GetAtomsAttribute(FString("worldSpace"), attributes);

}

void UAtomsXpuExampleJointTransformTaskNode::SetAtomsAttribute(AtomsCore::MapMetadata* attributes)
{
	jointName.SetAtomsAttribute(FString("jointName"), attributes);
	lockAgent.SetAtomsAttribute(FString("lockAgent"), attributes);
	mode.SetAtomsAttribute(FString("mode"), attributes);
	offset.SetAtomsAttribute(FString("offset"), attributes);
	rotation.SetAtomsAttribute(FString("rotation"), attributes);
	rotationOrder.SetAtomsAttribute(FString("rotationOrder"), attributes);
	scale.SetAtomsAttribute(FString("scale"), attributes);
	translation.SetAtomsAttribute(FString("translation"), attributes);
	weight.SetAtomsAttribute(FString("weight"), attributes);
	worldSpace.SetAtomsAttribute(FString("worldSpace"), attributes);

}

Atoms::Behaviour* UAtomsXpuExampleJointTransformTaskNode::GenerateAtomsBehaviour(AActor* Actor, UActorComponent* Component)
{
	AtomsXpuExampleJointTransformTaskNodeWrapper* behaviour = new AtomsXpuExampleJointTransformTaskNodeWrapper();
	behaviour->AgentGroup = Cast<AAtomsAgentGroup>(Actor);
	behaviour->Component = Cast<UBehaviourTree_BehaviourComponent>(Component);
	return behaviour;
}

////////////////
// wrapper
////////////////

AtomsXpuExampleJointTransformTaskNodeWrapper::AtomsXpuExampleJointTransformTaskNodeWrapper() : AtomsBehaviourTreeXPUNode(),
//agentId(-1),
jointName(),
translation(AtomsMath::Vector3(0.0, 0.0, 0.0)),
rotation(AtomsMath::Vector3(0.0, 0.0, 0.0)),
scale(AtomsMath::Vector3(1.0, 1.0, 1.0)),
weight(1.0),
rotationOrder(0),
mode(0),
offset(true),
worldSpace(false)
{

}

AtomsXpuExampleJointTransformTaskNodeWrapper::~AtomsXpuExampleJointTransformTaskNodeWrapper()
{

}

const char* AtomsXpuExampleJointTransformTaskNodeWrapper::staticTypeName()
{
	return "JointTransformXPU";
}

const char* AtomsXpuExampleJointTransformTaskNodeWrapper::typeName() const
{
	return staticTypeName();
}

Atoms::Behaviour* AtomsXpuExampleJointTransformTaskNodeWrapper::creator() { return new AtomsXpuExampleJointTransformTaskNodeWrapper(); }

size_t AtomsXpuExampleJointTransformTaskNodeWrapper::GpuDataStride() const
{
	return AtomsBehaviourTreeXPUNode::GpuDataStride() + 14;
}

AtomsCore::Euler::Order AtomsXpuExampleJointTransformTaskNodeWrapper::convertRotateOrderToEulerRot(int value)
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


void AtomsXpuExampleJointTransformTaskNodeWrapper::initialize(Atoms::BehaviourTreeContext* context, Atoms::Behaviour::State* data)
{
	data->status = Atoms::Behaviour::Status::FAILURE;
	data->ptrValue = nullptr;
	if (lock)
	{
		context->agent->lock();
	}

	auto& jointNameValue = context->getBlackboardValue(jointName);
	auto& translationValue = context->getBlackboardValue(translation);
	auto& rotationValue = context->getBlackboardValue(rotation);
	auto& rotationOrderValue = context->getBlackboardValue(rotationOrder);
	auto& scaleValue = context->getBlackboardValue(scale);
	auto& weightValue = context->getBlackboardValue(weight);
	auto& offsetValue = context->getBlackboardValue(offset);
	auto& worldSpaceValue = context->getBlackboardValue(worldSpace);
	auto& modeValue = context->getBlackboardValue(mode);

	auto JointId = context->agent->agentType()->skeleton().jointId(jointNameValue);
	if (JointId > 0)
	{

		//pass data to the behaviour component for executing on the gpu
		if (Component.IsValid())
		{
			data->status = Atoms::Behaviour::Status::RUNNING;

			std::string* nodeNamePtr = context->allocateMemory<std::string>();
			data->ptrValue = nodeNamePtr;
			*nodeNamePtr = "gpu";
			
			FScopeLock ScopeLock(&Component->GpuTaskMutex);
			auto bufferData = Component->RegisterGPUTask(this, context);
			check(bufferData.size() == 16);

			AtomsCore::Euler erotation(rotationValue.x * M_PI / 180.0, rotationValue.y * M_PI / 180.0, rotationValue.z * M_PI / 180.0, convertRotateOrderToEulerRot(rotationOrderValue));
			AtomsCore::Quaternion quatTmp = erotation.toQuat();

			// fill the buffer
			bufferData[0] = context->agent->groupId()->value();
			bufferData[1] = JointId;
			bufferData[2] = translationValue.x;
			bufferData[3] = -translationValue.y;
			bufferData[4] = translationValue.z;
			bufferData[5] = quatTmp.v.x;
			bufferData[6] = -quatTmp.v.y;
			bufferData[7] = quatTmp.v.z;
			bufferData[8] = -quatTmp.r;
			bufferData[9] = scaleValue.x;
			bufferData[10] = scaleValue.z;
			bufferData[11] = scaleValue.y;
			bufferData[12] = weightValue;
			bufferData[13] = offsetValue;
			bufferData[14] = worldSpaceValue;
			bufferData[15] = modeValue;
			
		}

		if (lock)
		{
			context->agent->unlock();
		}
		return;
	}


	const auto& nodeName = context->tree->names[index];

	Atoms::AgentBehaviourNetwork& network = context->agent->network();
	auto& manager = context->agent->network().manager();



	Atoms::JointTransformOperator* jointTransform = static_cast<Atoms::JointTransformOperator*>(network.manager().createNode(Atoms::JointTransformOperator::staticTypeStr(), nodeName + "_jointTransform"));
	if (!jointTransform)
	{
		if (lock)
		{
			context->agent->unlock();
		}
		return;
	}
	std::string* nodeNamePtr = context->allocateMemory<std::string>();
	data->ptrValue = nodeNamePtr;
	*nodeNamePtr = jointTransform->name();
	jointTransform->setAgent(context->agent);

	//AtomsCore::StringMetadata bm(jointTransform->name());
	//agent->metadata().addEntry("_" + name() + JOINT_TRANSFORM_OPERATOR_NAME, &bm);

	jointTransform->getInputPort<AtomsGraph::StringPort>("jointName")->set(jointNameValue);
	jointTransform->getInputPort<AtomsGraph::LongPort>("rotationOrder")->set(rotationOrderValue);
	jointTransform->getInputPort<AtomsGraph::BooleanPort>("offset")->set(offsetValue);
	jointTransform->getInputPort<AtomsGraph::BooleanPort>("worldSpace")->set(worldSpaceValue);

	jointTransform->getInputPort<AtomsGraph::VectorPort>("translate")->set(translationValue);
	jointTransform->getInputPort<AtomsGraph::VectorPort>("rotate")->set(rotationValue);
	jointTransform->getInputPort<AtomsGraph::VectorPort>("scale")->set(scaleValue);
	jointTransform->getInputPort<AtomsGraph::DoublePort>("weight")->set(weightValue);


	//end of chain
	if (modeValue == 0)
	{
		Atoms::Operator* endOperator = network.buildPoseNode();
		if (endOperator)
			network.manager().connectAttr(endOperator->name(), "outPose", jointTransform->name(), "inPose");
		network.setBuildPoseNode(jointTransform);
	}
	if (modeValue == 1) // pre ragdoll
	{
		auto pelvisOp = manager.getNode("ragdoll");
		if (!pelvisOp)
		{
			// if not ragdoll attach as last
			Atoms::Operator* endOperator = network.buildPoseNode();
			network.manager().connectAttr(endOperator->name(), "outPose", jointTransform->name(), "inPose");
			network.setBuildPoseNode(jointTransform);
		}
		else
		{
			auto pelvisIn = pelvisOp->getInputPort("inPose");
			auto inputPort = pelvisIn->input();
			if (inputPort)
			{
				//AtomsUtils::Logger::info() << "Disconnect " << inputPort->node()->name() << "." << inputPort->name() << " -> " << pelvisOperator->name() << "." << "inPose";
				manager.disconnectAttr(inputPort->node()->name(), inputPort->name(), pelvisOp->name(), "inPose");
				manager.connectAttr(inputPort->node()->name(), inputPort->name(), jointTransform->name(), "inPose");
			}

			manager.connectAttr(jointTransform->name(), "outPose", pelvisOp->name(), "inPose");
		}
	}
	if (modeValue == 2) // pre pelvis
	{
		auto pelvisOp = manager.getNode("pelvisOperator");
		if (!pelvisOp)
		{
			if (lock)
			{
				context->agent->unlock();
			}
			return;
		}

		auto pelvisIn = pelvisOp->getInputPort("inPose");
		auto inputPort = pelvisIn->input();
		if (inputPort)
		{
			//AtomsUtils::Logger::info() << "Disconnect " << inputPort->node()->name() << "." << inputPort->name() << " -> " << pelvisOperator->name() << "." << "inPose";
			manager.disconnectAttr(inputPort->node()->name(), inputPort->name(), pelvisOp->name(), "inPose");
			manager.connectAttr(inputPort->node()->name(), inputPort->name(), jointTransform->name(), "inPose");
		}

		manager.connectAttr(jointTransform->name(), "outPose", pelvisOp->name(), "inPose");
	}

	//This is important since we changed the graph so we invalidated the network cache
	manager.clearNodeNetworkCaches();
	context->agent->network().buildNetworkCaches();
	data->status = Status::RUNNING;

	if (lock)
	{
		context->agent->unlock();
	}
}

void AtomsXpuExampleJointTransformTaskNodeWrapper::update(Atoms::BehaviourTreeContext* context, Atoms::Behaviour::State* data)
{
	if (!data->ptrValue)
	{
		data->status = Status::FAILURE;
		return;
	}
	if (!context->agent)
	{
		data->status = Status::FAILURE;
		return;
	}

	auto& jointNameValue = context->getBlackboardValue(jointName);
	auto& translationValue = context->getBlackboardValue(translation);
	auto& rotationValue = context->getBlackboardValue(rotation);
	auto& scaleValue = context->getBlackboardValue(scale);
	auto& weightValue = context->getBlackboardValue(weight);
	auto& offsetValue = context->getBlackboardValue(offset);
	auto& worldSpaceValue = context->getBlackboardValue(worldSpace);
	auto& modeValue = context->getBlackboardValue(mode);
	auto& rotationOrderValue = context->getBlackboardValue(rotationOrder);

	std::string* namePtr = reinterpret_cast<std::string*>(data->ptrValue);
	if (Component.IsValid() && namePtr && *namePtr == "gpu")
	{
		FScopeLock ScopeLock(&Component->GpuTaskMutex);
		auto bufferData = Component->RegisterGPUTask(this, context);
		// fill the buffer
		auto JointId = context->agent->agentType()->skeleton().jointId(jointNameValue);

		AtomsCore::Euler erotation(rotationValue.x * M_PI / 180.0, rotationValue.y * M_PI / 180.0, rotationValue.z * M_PI / 180.0, convertRotateOrderToEulerRot(rotationOrderValue));
		AtomsCore::Quaternion quatTmp = erotation.toQuat();
		//quatTmp.invert();
		// fill the buffer
		bufferData[0] = context->agent->groupId()->value();
		bufferData[1] = JointId;
		bufferData[2] = translationValue.x;
		bufferData[3] = -translationValue.y;
		bufferData[4] = translationValue.z;
		bufferData[5] = quatTmp.v.x;
		bufferData[6] = -quatTmp.v.y;
		bufferData[7] = quatTmp.v.z;
		bufferData[8] = -quatTmp.r;
		bufferData[9] = scaleValue.x;
		bufferData[10] = scaleValue.y;
		bufferData[11] = scaleValue.z;
		bufferData[12] = weightValue;
		bufferData[13] = offsetValue;
		bufferData[14] = worldSpaceValue;
		bufferData[15] = modeValue;

		return;
	}



	auto& manager = context->agent->network().manager();
	auto jointTransform = manager.getNode(*namePtr);
	if (!jointTransform)
	{
		data->status = Status::FAILURE;
		return;
	}
	jointTransform->getInputPort<AtomsGraph::VectorPort>("translate")->set(translationValue);
	jointTransform->getInputPort<AtomsGraph::VectorPort>("rotate")->set(rotationValue);
}

void AtomsXpuExampleJointTransformTaskNodeWrapper::terminate(Atoms::BehaviourTreeContext* context, Atoms::Behaviour::State* data)
{
	if (!data->ptrValue || !context->agent)
	{
		return;
	}

	std::string* namePtr = reinterpret_cast<std::string*>(data->ptrValue);
	if (lock)
	{
		context->agent->lock();
	}

	Atoms::AgentBehaviourNetwork& network = context->agent->network();
	auto& manager = context->agent->network().manager();

	auto jointTransformOp = manager.getNode(*namePtr);
	if (!jointTransformOp)
	{
		if (lock)
		{
			context->agent->unlock();
		}
		return;
	}

	auto mergeIn = jointTransformOp->getInputPort("inPose");
	if (mergeIn)
	{
		auto inputPort = mergeIn->input();
		if (inputPort)
		{
			mergeIn->disconnectFrom(inputPort);
			if (network.buildPoseNode() == jointTransformOp)
			{
				network.setBuildPoseNode(static_cast<Atoms::Operator*>(inputPort->node()));
			}
		}

		// connect to the output node
		auto mergeOut = jointTransformOp->getOutputPort("outPose");
		if (mergeOut && mergeOut->numConnections() > 0)
		{
			auto outPort = mergeOut->getConnections()[0];
			outPort->disconnectFrom(mergeOut);

			manager.connectAttr(inputPort->node()->name(), inputPort->name(), outPort->node()->name(), outPort->name());
		}
	}

	manager.clearNodeNetworkCaches();

	if (jointTransformOp)
		manager.deleteNode(jointTransformOp->name());

	context->agent->network().buildNetworkCaches();

	if (lock)
	{
		context->agent->unlock();
	}
}

void AtomsXpuExampleJointTransformTaskNodeWrapper::releaseData(Atoms::BehaviourTreeContext* context, Atoms::Behaviour::State* data)
{
	if (data->ptrValue)
	{
		std::string* namePtr = reinterpret_cast<std::string*>(data->ptrValue);
		context->releaseMemory(namePtr);
		data->ptrValue = nullptr;
	}
}

void AtomsXpuExampleJointTransformTaskNodeWrapper::setAttributes(const AtomsCore::MapMetadata* attributes, Atoms::Blackboard* blackboard)
{
	if (!attributes)
		return;

	Atoms::Behaviour::setAttributes(attributes, blackboard);

	//getBlackboardValueFromAttributes<AtomsCore::IntMetadata, int>(attributes, blackboard, "agentId", agentId);
	Atoms::getBlackboardValueFromAttributes<AtomsCore::StringMetadata, std::string>(attributes, blackboard, "jointName", jointName);
	Atoms::getBlackboardValueFromAttributes<AtomsCore::Vector3Metadata, AtomsMath::Vector3>(attributes, blackboard, "translation", translation);
	Atoms::getBlackboardValueFromAttributes<AtomsCore::Vector3Metadata, AtomsMath::Vector3>(attributes, blackboard, "rotation", rotation);
	Atoms::getBlackboardValueFromAttributes<AtomsCore::IntMetadata, int>(attributes, blackboard, "rotationOrder", rotationOrder);
	Atoms::getBlackboardValueFromAttributes<AtomsCore::Vector3Metadata, AtomsMath::Vector3>(attributes, blackboard, "scale", scale);
	Atoms::getBlackboardValueFromAttributes<AtomsCore::DoubleMetadata, double>(attributes, blackboard, "weight", weight);
	Atoms::getBlackboardValueFromAttributes<AtomsCore::BoolMetadata, bool>(attributes, blackboard, "offset", offset);
	Atoms::getBlackboardValueFromAttributes<AtomsCore::BoolMetadata, bool>(attributes, blackboard, "worldSpace", worldSpace);
	Atoms::getBlackboardValueFromAttributes<AtomsCore::IntMetadata, int>(attributes, blackboard, "mode", mode);
}

void AtomsXpuExampleJointTransformTaskNodeWrapper::getAttributes(AtomsCore::MapMetadata* attributes, Atoms::Blackboard* blackboard)
{
	if (!attributes)
		return;

	Atoms::Behaviour::getAttributes(attributes, blackboard);

	AtomsCore::StringMetadata jointNameMeta(jointName.value);
	attributes->addEntry("jointName", &jointNameMeta);

	AtomsCore::Vector3Metadata translationMeta(translation.value);
	attributes->addEntry("translation", &translationMeta);

	AtomsCore::Vector3Metadata rotationMeta(rotation.value);
	attributes->addEntry("rotation", &rotationMeta);

	AtomsCore::Vector3Metadata scaleMeta(scale.value);
	attributes->addEntry("scale", &scaleMeta);

	AtomsCore::DoubleMetadata weightMeta(weight.value);
	attributes->addEntry("weight", &weightMeta);

	AtomsCore::BoolMetadata offsetMeta(offset.value);
	attributes->addEntry("offset", &offsetMeta);

	AtomsCore::BoolMetadata worldSpaceMeta(worldSpace.value);
	attributes->addEntry("worldSpace", &worldSpaceMeta);

	AtomsCore::IntMetadata modeMeta(mode.value);
	attributes->addEntry("mode", &modeMeta);

	AtomsCore::IntMetadata rotationOrderMeta(rotationOrder.value);
	attributes->addEntry("rotationOrder", &rotationOrderMeta);
}

void AtomsXpuExampleJointTransformTaskNodeWrapper::getAttributeProperties(AtomsCore::MapMetadata* attributes)
{
	if (!attributes)
		return;

	Atoms::Behaviour::getAttributeProperties(attributes);
	auto parameters = attributes->getTypedEntry<AtomsCore::MapMetadata>(Atoms::GlobalNameKeys::ATOMS_BEHAVIOUR_TREE_NODE_PARAMETERS_KEY);

	AtomsCore::StringMetadata description("Sets/Adds a transformation to a joint.<br/><br/><b>Success</b>: Never.<br/><br/><b>Failure</b>: When the transformation could not be applied.<br/><br/><b>Running</b>: When the transformation is applied.");
	attributes->addEntry(Atoms::GlobalNameKeys::ATOMS_BEHAVIOUR_TREE_NODE_DESCRIPTION_KEY, &description);

	AtomsCore::StringMetadata tooltip;

	AtomsCore::MapMetadata attributeP;

	tooltip.set("Joint name");
	attributeP.addEntry(Atoms::GlobalNameKeys::ATOMS_BEHAVIOUR_TREE_PARAMETER_DESCRIPTION_KEY, &tooltip);
	parameters->addEntry("jointName", &attributeP);

	tooltip.set("Translation");
	attributeP.addEntry(Atoms::GlobalNameKeys::ATOMS_BEHAVIOUR_TREE_PARAMETER_DESCRIPTION_KEY, &tooltip);
	parameters->addEntry("translation", &attributeP);

	tooltip.set("Rotation");
	attributeP.addEntry(Atoms::GlobalNameKeys::ATOMS_BEHAVIOUR_TREE_PARAMETER_DESCRIPTION_KEY, &tooltip);
	parameters->addEntry("rotation", &attributeP);

	tooltip.set("Scale");
	attributeP.addEntry(Atoms::GlobalNameKeys::ATOMS_BEHAVIOUR_TREE_PARAMETER_DESCRIPTION_KEY, &tooltip);
	parameters->addEntry("scale", &attributeP);

	tooltip.set("Blending weight");
	attributeP.addEntry(Atoms::GlobalNameKeys::ATOMS_BEHAVIOUR_TREE_PARAMETER_DESCRIPTION_KEY, &tooltip);
	parameters->addEntry("weight", &attributeP);

	tooltip.set("Apply the transformation as an offset");
	attributeP.addEntry(Atoms::GlobalNameKeys::ATOMS_BEHAVIOUR_TREE_PARAMETER_DESCRIPTION_KEY, &tooltip);
	parameters->addEntry("offset", &attributeP);

	tooltip.set("Apply the transformation in world space");
	attributeP.addEntry(Atoms::GlobalNameKeys::ATOMS_BEHAVIOUR_TREE_PARAMETER_DESCRIPTION_KEY, &tooltip);
	parameters->addEntry("worldSpace", &attributeP);

	AtomsCore::StringMetadata comboBoxM(Atoms::GlobalNameKeys::ATOMS_BEHAVIOUR_TREE_HARD_CONSTRUCTOR_COMBOBOX_KEY);
	attributeP.addEntry(Atoms::GlobalNameKeys::ATOMS_BEHAVIOUR_TREE_HARD_CONSTRUCTOR_KEY, &comboBoxM);
	std::vector<std::string> items{ "As last operator", "Pre-physic operator", "Pre-pelvis operator" };
	AtomsCore::StringArrayMetadata optionList(items);
	attributeP.addEntry(Atoms::GlobalNameKeys::ATOMS_BEHAVIOUR_TREE_OPTION_LIST_KEY, &optionList);
	tooltip.set("Mode:<br/>0: As last operator<br/>1: Pre-physic operator<br/>2: Pre-pelvis operator");
	attributeP.addEntry(Atoms::GlobalNameKeys::ATOMS_BEHAVIOUR_TREE_PARAMETER_DESCRIPTION_KEY, &tooltip);
	parameters->addEntry("mode", &attributeP);

	optionList.get() = { "XYZ", "YZX", "ZXY", "XZY", "YXZ", "ZYX" };
	attributeP.addEntry(Atoms::GlobalNameKeys::ATOMS_BEHAVIOUR_TREE_OPTION_LIST_KEY, &optionList);
	tooltip.set("Rotation order:<br/>0: XYZ<br/>1: YZX<br/>2: ZXY<br/>3: XZY<br/>4: YXZ<br/>5: ZYX");
	attributeP.addEntry(Atoms::GlobalNameKeys::ATOMS_BEHAVIOUR_TREE_PARAMETER_DESCRIPTION_KEY, &tooltip);
	parameters->addEntry("rotationOrder", &attributeP);
}

TSharedPtr<FAtomsBehaviourTreeXPUDispatcher, ESPMode::ThreadSafe> AtomsXpuExampleJointTransformTaskNodeWrapper::CreateDispatcher()
{
	return MakeShared<FAtomsXpuExampleJointTransformXPUDispatcher, ESPMode::ThreadSafe>();
}

#undef LOCTEXT_NAMESPACE
