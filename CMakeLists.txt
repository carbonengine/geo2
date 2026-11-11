cmake_minimum_required(VERSION 3.31)

option(BUILD_TESTING "Build tests. Enabled by default." ON)
if(BUILD_TESTING)
    list(APPEND VCPKG_MANIFEST_FEATURES "tests")
endif()

project(geo2)

include(cmake/CcpTargetConfigurations.cmake)
include(cmake/CcpDocsGenerator.cmake)
include(cmake/CcpPackageConfigHelpers.cmake)
include(cmake/CcpBuildConfigurations.cmake)

ccp_add_library(geo2 SHARED)

find_package(Python3 COMPONENTS Development REQUIRED)
find_package(carbon-core CONFIG REQUIRED)
find_package(carbon-math CONFIG REQUIRED)

target_link_libraries(geo2 PRIVATE Python3::Python CcpMath CcpCore)

file(GLOB SRC_FILES "src/*")
target_sources(geo2 PRIVATE
    ${SRC_FILES}
)

set_target_properties(geo2 PROPERTIES OUTPUT_NAME "_geo2")


if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    if(BUILD_TESTING)
        include(CTest)
        find_package(carbon-exefile-interpreter REQUIRED CONFIG)
        # Interpreter must be created in this scope in order to create a POST_BUILD target for geo2.
        ccp_prepare_interpreter(
                TARGET geo2
                BUILD_TYPE ${CMAKE_BUILD_TYPE}
                COMMAND_OUT EXEFILE_INTERPRETER
                PYTHONPATH_OUT PYTHONPATH
                RUN_DIR $<TARGET_FILE_DIR:geo2>
                BLUE_DIR_OUT BLUE_LOCATION_DIR
        )
        add_subdirectory(tests)
    endif()

    option (INSTALL_TO_MONOLITH "Install this package to a perforce branch vendor folder - provided for backwards compatability" OFF)
    if(INSTALL_TO_MONOLITH)
        # Install rule to ensure that our runtime and linker files are in the expected, platform-specific folders
        install(
                TARGETS geo2
                CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES}
                ARCHIVE DESTINATION lib/${CCP_PLATFORM}/${CCP_ARCHITECTURE}/${CCP_TOOLSET}/
                LIBRARY DESTINATION lib/${CCP_PLATFORM}/${CCP_ARCHITECTURE}/${CCP_TOOLSET}/
                RUNTIME DESTINATION bin/${CCP_PLATFORM}/${CCP_ARCHITECTURE}/${CCP_TOOLSET}/
        )
        # Install python packages
        install(DIRECTORY python/ DESTINATION python
                FILES_MATCHING
                PATTERN "*.py"
                PATTERN "package.yaml"
        )
    else() # VCPKG install mode
        install(
            TARGETS geo2
            EXPORT geo2Targets
            CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES}
            ARCHIVE DESTINATION lib/
            LIBRARY DESTINATION lib/
            RUNTIME DESTINATION bin/
        )

        # Install python packages
        install(DIRECTORY python/ DESTINATION bin/python
                FILES_MATCHING
                PATTERN "*.py"
        )

        install(
                EXPORT geo2Targets
                DESTINATION ${CMAKE_INSTALL_PREFIX}/share/carbon-geo2
                FILE carbon-geo2Config.cmake
        )
    endif()
endif()
