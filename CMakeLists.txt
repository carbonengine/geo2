cmake_minimum_required(VERSION 3.31)

project(geo2)

include(cmake/CcpGlobalSettings.cmake)
include(cmake/CcpTargetConfigurations.cmake)

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
    install(
        TARGETS geo2
        EXPORT geo2Targets
        CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES}
        ARCHIVE DESTINATION lib/
        LIBRARY DESTINATION lib/
        RUNTIME DESTINATION bin/
    )

    # Install python packages
    install(DIRECTORY python/ DESTINATION python
            FILES_MATCHING
            PATTERN "*.py"
    )

    install(
            EXPORT geo2Targets
            DESTINATION ${CMAKE_INSTALL_PREFIX}/share/carbon-geo2
            FILE carbon-geo2Config.cmake
    )
endif()
