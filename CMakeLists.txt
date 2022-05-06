cmake_minimum_required(VERSION 3.14)

include(cmake/insource_guard.cmake)
include(cmake/vcpkg_integration.cmake)
include(cmake/files.cmake)
include(cmake/compiler_options.cmake)

project(
  "xml-raytracer"
  VERSION 0.1.0
  DESCRIPTION "xml-raytracer description"
  HOMEPAGE_URL "xinoip.github.io"
  LANGUAGES CXX
)

# == Dependencies ==

find_package(tinyxml2 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(Threads REQUIRED)

# == Library Part ==

add_library(
  xml-raytracer_lib OBJECT
  ${lib_src_list}
)

target_include_directories(
  xml-raytracer_lib
  PUBLIC
  "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)

target_compile_features(xml-raytracer_lib PUBLIC cxx_std_20)
target_compile_options(xml-raytracer_lib PUBLIC ${COMPILE_OPTIONS})

target_link_libraries(xml-raytracer_lib PRIVATE fmt::fmt)
target_link_libraries(xml-raytracer_lib PRIVATE tinyxml2::tinyxml2)
target_link_libraries(xml-raytracer_lib PRIVATE Threads::Threads)

# == Executable Part ==

add_executable(xml-raytracer_exe ${exe_src_list})

set_target_properties(
    xml-raytracer_exe PROPERTIES
    OUTPUT_NAME xml-raytracer
    EXPORT_NAME exe
)

target_compile_features(xml-raytracer_exe PRIVATE cxx_std_20)
target_compile_options(xml-raytracer_exe PUBLIC ${COMPILE_OPTIONS})

target_link_libraries(xml-raytracer_exe PRIVATE fmt::fmt)
target_link_libraries(xml-raytracer_exe PRIVATE tinyxml2::tinyxml2)
target_link_libraries(xml-raytracer_exe PRIVATE Threads::Threads)
target_link_libraries(xml-raytracer_exe PRIVATE xml-raytracer_lib)
