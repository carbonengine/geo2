cmake_minimum_required(VERSION 3.31)

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
