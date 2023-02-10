cmake_minimum_required(VERSION 3.16)
project(geo2 VERSION 1.0.0)
if(NOT DEFINED ENV{CCP_EVE_PERFORCE_BRANCH_PATH})
    message(FATAL_ERROR "Missing required environment variable CCP_EVE_PERFORCE_BRANCH_PATH")
elseif(NOT IS_ABSOLUTE $ENV{CCP_EVE_PERFORCE_BRANCH_PATH})
    message(FATAL_ERROR "CCP_EVE_PERFORCE_BRANCH_PATH needs to be an absolute path")
elseif(NOT EXISTS $ENV{CCP_EVE_PERFORCE_BRANCH_PATH})
    message(FATAL_ERROR "CCP_EVE_PERFORCE_BRANCH_PATH is pointing at a non-existing location $ENV{CCP_EVE_PERFORCE_BRANCH_PATH}")
elseif(NOT IS_DIRECTORY $ENV{CCP_EVE_PERFORCE_BRANCH_PATH})
    message(FATAL_ERROR "CCP_EVE_PERFORCE_BRANCH_PATH needs to be a directory")
else()
    message(STATUS "Using CCP_EVE_PERFORCE_BRANCH_PATH located at $ENV{CCP_EVE_PERFORCE_BRANCH_PATH}")
endif()

file(TO_CMAKE_PATH "$ENV{CCP_EVE_PERFORCE_BRANCH_PATH}" BRANCH_ROOT_DIR)
list(INSERT CMAKE_MODULE_PATH 0 ${BRANCH_ROOT_DIR}/cmake)

include(CcpGlobalSettings)

add_library(geo2 SHARED)

find_package(Python REQUIRED NO_CMAKE_PATH)
find_package(CcpMath REQUIRED NO_CMAKE_PATH)
target_link_libraries(geo2 PRIVATE Python CcpMath)

file(GLOB SRC_FILES "src/*")

target_sources(geo2 PRIVATE
    ${SRC_FILES}
)

if(APPLE)
    target_link_libraries(geo2 PRIVATE CcpCore)
endif()
set_target_properties(geo2 PROPERTIES OUTPUT_NAME "_geo2")
