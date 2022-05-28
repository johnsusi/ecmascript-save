install(EXPORT  "${CMAKE_PROJECT_NAME}Targets"
    FILE        "${CMAKE_PROJECT_NAME}Targets.cmake"
    COMPONENT   "${CMAKE_PROJECT_NAME}_Development"
    NAMESPACE   "${CMAKE_PROJECT_NAME}::"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}"
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file("${CMAKE_PROJECT_NAME}ConfigVersion.cmake"
    VERSION "${CMAKE_PROJECT_VERSION}"
    COMPATIBILITY SameMajorVersion
)

configure_file(cmake/config.cmake.in "${CMAKE_PROJECT_NAME}Config.cmake" @ONLY)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}"
)

set(CPACK_PACKAGE_VENDOR              "John Susi")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${CMAKE_PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_VERSION_MAJOR       "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR       "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH       "${PROJECT_VERSION_PATCH}")
set(CPACK_RESOURCE_FILE_LICENSE       "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README        "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

include(CPack)