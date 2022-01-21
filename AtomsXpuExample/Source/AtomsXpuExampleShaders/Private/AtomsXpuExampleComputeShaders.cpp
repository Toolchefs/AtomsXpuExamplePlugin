#include "AtomsXpuExampleComputeShaders.h"


IMPLEMENT_GLOBAL_SHADER(FAtomsXpuExampleJointTransformCS, "/Plugins/AtomsXpuExample/Private/AtomsXpuExampleJointTransformCompute.usf", "MainComputeShader", SF_Compute);

