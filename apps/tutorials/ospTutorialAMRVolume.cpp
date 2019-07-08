// ======================================================================== //
// Copyright 2018-2019 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#include <iterator>
#include <memory>
#include <random>
#include "GLFWOSPRayWindow.h"

#include "ospcommon/FileName.h"
#include "ospcommon/library.h"
#include "ospcommon/range.h"
#include "ospcommon/utility/ArrayView.h"
#include "ospray_testing.h"

#include "tutorial_util.h"

#include <imgui.h>

using namespace ospcommon;

static const std::string renderer_type = "scivis";

// AMRVolume context info
static std::vector<float *> brickPtrs;  // holds actual data
static std::vector<OSPData> brickData;  // holds actual data as OSPData
static range1f valueRange;
static box3f bounds;

int main(int argc, const char **argv)
{
  initializeOSPRay(argc, argv);

  // create the world
  OSPWorld world = ospNewWorld();

  // create the instance that will contain our volume model
  OSPInstance instance = ospNewInstance();

  // generate an AMR volume
  OSPTestingVolume testData = ospTestingNewVolume("gravity_spheres_amr_volume");

  OSPVolume amrVolume = testData.volume;

  osp_vec2f amrValueRange = testData.voxelRange;
  OSPTransferFunction tf  = ospTestingNewTransferFunction(amrValueRange, "jet");

  // create the model that will contain our actual volume
  OSPVolumetricModel volumeModel = ospNewVolumetricModel(amrVolume);
  ospSetObject(volumeModel, "transferFunction", tf);
  ospSetFloat(volumeModel, "samplingRate", 10.f);
  ospCommit(volumeModel);

  // create a data array of all models for the instance
  OSPData volumeModels = ospNewData(1, OSP_OBJECT, &volumeModel);
  ospSetObject(instance, "volumes", volumeModels);
  ospCommit(instance);
  ospRelease(volumeModels);

  // create a data array of all instances for the world
  OSPData volumeInstances = ospNewData(1, OSP_OBJECT, &instance);
  ospSetData(world, "instances", volumeInstances);
  ospRelease(volumeInstances);

  // create OSPRay renderer
  OSPRenderer renderer = ospNewRenderer(renderer_type.c_str());

  OSPData lightsData = ospTestingNewLights("ambient_only");
  ospSetData(renderer, "lights", lightsData);
  ospRelease(lightsData);

  ospCommit(renderer);

  bounds = reinterpret_cast<box3f &>(testData.bounds);

  // commit the world
  ospCommit(world);

  // create a GLFW OSPRay window: this object will create and manage the OSPRay
  // frame buffer and camera directly
  auto glfwOSPRayWindow = std::unique_ptr<GLFWOSPRayWindow>(
      new GLFWOSPRayWindow(vec2i{1024, 768}, bounds, world, renderer));

  // start the GLFW main loop, which will continuously render
  glfwOSPRayWindow->mainLoop();

  // cleanly shut OSPRay down
  ospShutdown();

  return 0;
}