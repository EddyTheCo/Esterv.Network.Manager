if(BUILD_EXAMPLES)
  include(InstallRequiredSystemLibraries)

  foreach(example "Wifilist")
    qt_add_executable(${example} ${example}.cpp)

    qt6_add_qml_module(
      ${example}
      URI
      Examples${example}
      VERSION
      1.0
      QML_FILES
      "qml/${example}.qml"
      RESOURCE_PREFIX
      "/esterVtech.com/imports"
      IMPORT_PATH
      ${CMAKE_BINARY_DIR})

    target_link_libraries(
      ${example}
      PRIVATE
        Qt::Gui
        Qt::Qml
        Qt::Quick
        NetworkManager
        $<$<STREQUAL:$<TARGET_PROPERTY:NetworkManager,TYPE>,STATIC_LIBRARY>:NetworkManagerplugin>
    )

    install(
      TARGETS ${example}
      COMPONENT ${PROJECT_NAME}-examples
      BUNDLE DESTINATION . COMPONENT ${PROJECT_NAME}-examples
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
              COMPONENT ${PROJECT_NAME}-examples)
    if(QTDEPLOY)
      qt_generate_deploy_qml_app_script(TARGET ${example} OUTPUT_SCRIPT
                                        deploy_script)
      install(SCRIPT ${deploy_script} COMPONENT ${PROJECT_NAME}-examples)
    endif(QTDEPLOY)
  endforeach()
endif(BUILD_EXAMPLES)
