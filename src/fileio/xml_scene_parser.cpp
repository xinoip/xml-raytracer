#include "xml_scene_parser.hpp"

#include <tinyxml2.h>
#include <fmt/core.h>
#include <sstream>

namespace XmlRaytracer {

static std::vector<double> take_n_number(const char* txt, int n) {
    std::vector<double> rtr{};
    std::stringstream stream(txt);
    for (int i = 0; stream && i < n; i++) {
        double num;
        stream >> num;
        rtr.push_back(num);
    }
    return rtr;
}

static bool try_to_fill_vector_from_xml(tinyxml2::XMLElement* element,
                                        Vec3& vec,
                                        const std::string& tag_name) {
    if (!element) {
        fmt::print("<{}> tag not found in xml!\n", tag_name);
        return false;
    }

    auto numbers = take_n_number(element->GetText(), 3);
    vec.x = numbers[0];
    vec.y = numbers[1];
    vec.z = numbers[2];
    return true;
}

bool create_scene_from_xml(const std::string& path, Scene& scene) {
    using namespace tinyxml2;

    XMLDocument doc;
    fmt::print("Loading scene file...\n");
    XMLError res = doc.LoadFile(path.c_str());
    if (res != XMLError::XML_SUCCESS) {
        fmt::print("Scene file couldn't have been loaded\n");
        return false;
    }
    fmt::print("Scene file loaded successfully\n");

    XMLElement* xml_scene = doc.FirstChildElement("scene");
    if (!xml_scene) {
        fmt::print("<scene> tag not found in xml!\n");
        return false;
    }

    XMLElement* xml_max_raytrace_depth =
        xml_scene->FirstChildElement("maxraytracedepth");
    if (xml_max_raytrace_depth) {
        scene.max_raytrace_depth = xml_max_raytrace_depth->IntText();
    } else {
        fmt::print("<maxraytracedepth> tag not found in xml!\n");
    }

    XMLElement* xml_background = xml_scene->FirstChildElement("background");
    try_to_fill_vector_from_xml(xml_background, scene.background, "background");

    XMLElement* xml_camera = xml_scene->FirstChildElement("camera");
    if (xml_camera) {
        XMLElement* xml_camera_position =
            xml_camera->FirstChildElement("position");
        try_to_fill_vector_from_xml(
            xml_camera_position, scene.camera.position, "camera>position");

        XMLElement* xml_camera_gaze = xml_camera->FirstChildElement("gaze");
        try_to_fill_vector_from_xml(
            xml_camera_gaze, scene.camera.gaze, "camera>gaze");

        XMLElement* xml_camera_up = xml_camera->FirstChildElement("up");
        try_to_fill_vector_from_xml(
            xml_camera_up, scene.camera.up, "camera>up");

        XMLElement* xml_camera_nearplane =
            xml_camera->FirstChildElement("nearplane");
        if (xml_camera_nearplane) {
            auto numbers = take_n_number(xml_camera_nearplane->GetText(), 4);
            scene.camera.l = numbers[0];
            scene.camera.r = numbers[1];
            scene.camera.t = numbers[2];
            scene.camera.b = numbers[3];
        } else {
            fmt::print("<camera>nearplane> tag not found in xml!\n");
        }

        XMLElement* xml_camera_neardistance =
            xml_camera->FirstChildElement("neardistance");
        if (xml_camera_neardistance) {
            scene.camera.distance = xml_camera_neardistance->DoubleText();
        } else {
            fmt::print("<camera>neardistance> tag not found in xml!\n");
        }

        XMLElement* xml_camera_imageresolution =
            xml_camera->FirstChildElement("imageresolution");
        if (xml_camera_imageresolution) {
            auto numbers =
                take_n_number(xml_camera_imageresolution->GetText(), 2);
            scene.camera.nx = static_cast<int>(numbers[0]);
            scene.camera.ny = static_cast<int>(numbers[1]);
        } else {
            fmt::print("<camera>imageresolution> tag not found in xml!\n");
        }
    } else {
        fmt::print("<camera> tag not found in xml!\n");
    }

    XMLElement* xml_lights = xml_scene->FirstChildElement("lights");
    if (xml_lights) {
        XMLElement* xml_lights_ambientlight =
            xml_lights->FirstChildElement("ambientlight");
        try_to_fill_vector_from_xml(xml_lights_ambientlight,
                                    scene.ambient_light,
                                    "lights>ambientlight");

        XMLElement* curr = xml_lights->FirstChildElement("pointlight");
        if (!curr) {
            fmt::print("<lights>pointlight> not found in xml!\n");
        }
        while (curr) {
            Light light{};
            light.id = curr->IntAttribute("id");
            XMLElement* position = curr->FirstChildElement("position");
            try_to_fill_vector_from_xml(
                position, light.position, "lights>pointlight>position");

            XMLElement* intensity = curr->FirstChildElement("intensity");
            try_to_fill_vector_from_xml(
                intensity, light.intensity, "lights>pointlight>intensity");
            scene.lights.push_back(light);
            curr = curr->NextSiblingElement("pointlight");
        }
    } else {
        fmt::print("<lights> tag not found in xml!\n");
    }

    XMLElement* xml_materials = xml_scene->FirstChildElement("materials");
    if (xml_materials) {
        XMLElement* curr = xml_materials->FirstChildElement("material");
        if (!curr) {
            fmt::print("<materials>material> not found in xml!\n");
        }
        while (curr) {
            Material material{};
            material.id = curr->IntAttribute("id");
            XMLElement* ambient = curr->FirstChildElement("ambient");
            try_to_fill_vector_from_xml(
                ambient, material.ambient, "materials>material>ambient");

            XMLElement* diffuse = curr->FirstChildElement("diffuse");
            try_to_fill_vector_from_xml(
                diffuse, material.diffuse, "materials>material>diffuse");

            XMLElement* specular = curr->FirstChildElement("specular");
            try_to_fill_vector_from_xml(
                specular, material.specular, "materials>material>specular");

            XMLElement* mirrorreflectance =
                curr->FirstChildElement("mirrorreflectance");
            try_to_fill_vector_from_xml(mirrorreflectance,
                                        material.mirror_reflectance,
                                        "materials>material>mirrorreflectance");

            XMLElement* phongexponent =
                curr->FirstChildElement("phongexponent");
            if (phongexponent) {
                material.phong_exponent = phongexponent->IntText();
            } else {
                fmt::print("<materials>material>phongexponent> tag not found "
                           "in xml!\n");
            }

            scene.materials.push_back(material);
            curr = curr->NextSiblingElement();
        }
    } else {
        fmt::print("<materials> tag not found in xml!\n");
    }

    XMLElement* xml_vertex_data = xml_scene->FirstChildElement("vertexdata");
    if (xml_vertex_data) {
        const char* txt = xml_vertex_data->GetText();
        std::stringstream stream(txt);
        int i = 0;
        Vec3 curr_vec{};
        while (stream) {
            double num;
            stream >> num;
            if (i >= 3) {
                i = 0;
                scene.vertex_data.push_back(curr_vec);
            }
            curr_vec[i] = num;
            i++;
        }
    } else {
        fmt::print("<vertexdata> tag not found in xml!\n");
    }

    XMLElement* xml_objects = xml_scene->FirstChildElement("objects");
    if (xml_objects) {
        XMLElement* curr = xml_objects->FirstChildElement("mesh");
        if (!curr) {
            fmt::print("<objects>mesh> not found in xml!\n");
        }
        while (curr) {
            Mesh mesh{};
            mesh.id = curr->IntAttribute("id");
            XMLElement* material_id = curr->FirstChildElement("materialid");
            if (material_id) {
                mesh.material_id = material_id->IntText();
            } else {
                fmt::print("<objects>mesh>materialid> not found in xml!\n");
            }

            XMLElement* mesh_faces = curr->FirstChildElement("faces");
            if (mesh_faces) {
                const char* txt = mesh_faces->GetText();
                std::stringstream stream(txt);
                int i = 0;
                Vec3 curr_vec{};
                while (stream) {
                    double num;
                    stream >> num;
                    if (i >= 3) {
                        i = 0;
                        mesh.faces.push_back(curr_vec);
                    }
                    curr_vec[i] = num;
                    i++;
                }
            } else {
                fmt::print("<objects>mesh>faces> tag not found in xml!\n");
            }

            scene.objects.push_back(mesh);
            curr = curr->NextSiblingElement();
        }
    } else {
        fmt::print("<objects> tag not found in xml!\n");
    }

    return true;
}
} // namespace XmlRaytracer
