cmake_minimum_required(VERSION 3.24)

option(BUILD_EXAMPLES "Build the examples" ON)
option(QTDEPLOY "Deploy Qt libraries" OFF)

include(${CMAKE_CURRENT_BINARY_DIR}/local_conf.cmake OPTIONAL)

include(FetchContent)
FetchContent_Declare(
  ccommon
  GIT_REPOSITORY https://github.com/EddyTheCo/Common.git
  GIT_TAG develop)
FetchContent_MakeAvailable(ccommon)
version_from_git(LOG ON TIMESTAMP "%Y%m%d%H%M%S")
project(
  EstervNetworkManager
  VERSION ${VERSION}
  DESCRIPTION "QML library for Network Management"
  LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
include(GNUInstallDirs)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick ShaderTools)
qt_policy(SET QTP0004 NEW)
FetchContent_Declare(
  EstervDesigns
  GIT_REPOSITORY https://github.com/EddyTheCo/Esterv.Designs.git
  GIT_TAG v2.0.0
  FIND_PACKAGE_ARGS 2 COMPONENTS SimpleStyle CONFIG)
FetchContent_MakeAvailable(EstervDesigns)

FetchContent_Declare(
  GDbusCpp
  GIT_REPOSITORY git@github.com:amarula/libcppconnman.git
  GIT_TAG main
  FIND_PACKAGE_ARGS 1.1 CONFIG)
set(BUILD_CONNMAN ON)
FetchContent_MakeAvailable(GDbusCpp)

qt_standard_project_setup()
qt6_add_qml_module(
  NetworkManager
  URI
  Esterv.CustomControls.NetworkManager
  VERSION
  1.0
  SOURCES
  src/network_manager.cpp
  include/esterv/utils/network_manger.hpp
  QML_FILES
  "qml/WifiList.qml"
  "qml/Wifiicon.qml"
  RESOURCE_PREFIX
  "/esterVtech.com/imports"
  OUTPUT_TARGETS
  out_targets_var
  OUTPUT_DIRECTORY
  ${CMAKE_BINARY_DIR}/Esterv/CustomControls/NetworkManager
  IMPORT_PATH
  ${CMAKE_BINARY_DIR})

add_library(Esterv::NetworkManager ALIAS NetworkManager)
add_library(Esterv::NetworkManagerplugin ALIAS NetworkManagerplugin)
target_compile_definitions(
  NetworkManager
  PUBLIC
    $<IF:$<AND:$<BOOL:WIN32>,$<STREQUAL:$<TARGET_PROPERTY:NetworkManager,TYPE>,SHARED_LIBRARY>>,NETWORKMMANAGER_SHARED,>
)
target_compile_definitions(NetworkManager PRIVATE WINDOWS_EXPORT)
target_compile_definitions(NetworkManager PRIVATE QT_NO_KEYWORDS)
set_target_properties(NetworkManager PROPERTIES VERSION ${VERSION}
                                                SOVERSION ${VERSION_MAJOR})

qt6_add_shaders(
  NetworkManager
  "esterVtech.com.imports.NetworkManager.shaders"
  BATCHABLE
  PRECOMPILE
  OPTIMIZED
  OUTPUT_TARGETS
  out_targets_var2
  PREFIX
  "/esterVtech.com/imports/Designs"
  FILES
  "frag/wifi.frag")
target_include_directories(
  NetworkManager PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
target_include_directories(
  NetworkManager PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/esterv/utils)

target_link_libraries(
  NetworkManager
  PUBLIC
    Qt6::Gui
    Qt6::Quick
    Amarula::GConnmanDbus
    Esterv::SimpleStyle
    $<$<STREQUAL:$<TARGET_PROPERTY:Esterv::SimpleStyle,TYPE>,STATIC_LIBRARY>:Esterv::SimpleStyleplugin>
)

install(
  TARGETS NetworkManager ${out_targets_var} ${out_targets_var2}
  EXPORT ${PROJECT_NAME}-config
  COMPONENT EstervNetworkManager
  ARCHIVE COMPONENT ${PROJECT_NAME}-dev)
install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  COMPONENT ${PROJECT_NAME}-dev)
install(
  DIRECTORY ${CMAKE_BINARY_DIR}/Esterv/CustomControls/NetworkManager
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/Esterv/CustomControls
  COMPONENT EstervNetworkManager-qml)

install(
  TARGETS NetworkManagerplugin
  EXPORT ${PROJECT_NAME}-config
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/Esterv/CustomControls/NetworkManager
  COMPONENT EstervNetworkManager-qml
  ARCHIVE COMPONENT ${PROJECT_NAME}-dev)

if(QTDEPLOY
   AND NOT EMSCRIPTEN
   AND NOT ANDROID
   AND NOT CMAKE_CROSSCOMPILING)
  qt_generate_deploy_qml_app_script(TARGET NetworkManager OUTPUT_SCRIPT
                                    deploy_script)
  install(SCRIPT ${deploy_script} COMPONENT EstervNetworkManager)
endif(
  QTDEPLOY
  AND NOT EMSCRIPTEN
  AND NOT ANDROID
  AND NOT CMAKE_CROSSCOMPILING)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  add_subdirectory(examples)
endif(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

build_exports()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  build_cpack()
endif(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
if(BUILD_DOCS)
  build_docs()
endif(BUILD_DOCS)
