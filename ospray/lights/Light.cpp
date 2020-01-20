// ======================================================================== //
// Copyright 2009-2019 Intel Corporation                                    //
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

// ospray
#include "Light.h"
#include "Light_ispc.h"
#include "common/Util.h"

namespace ospray {

Light::Light()
{
  managedObjectType = OSP_LIGHT;
}

void Light::commit()
{
  const vec3f radiance =
      getParam<vec3f>("color", vec3f(1.f)) * getParam<float>("intensity", 1.f);
  ispc::Light_set(
      getIE(), (ispc::vec3f &)radiance, getParam<bool>("visible", true));
}

std::string Light::toString() const
{
  return "ospray::Light";
}

Light *Light::createInstance(const char *type)
{
  return createInstanceHelper<Light, OSP_LIGHT>(type);
}

OSPTYPEFOR_DEFINITION(Light *);

} // namespace ospray
