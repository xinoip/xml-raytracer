#pragma once

#include "scene.hpp"

namespace XmlRaytracer {

bool create_scene_from_xml(const std::string& path, Scene& scene);

} // namespace XmlRaytracer
