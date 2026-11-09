cmake_minimum_required(VERSION 3.31)

project(geo2)

include(cmake/CcpTargetConfigurations.cmake)
include(cmake/CcpDocsGenerator.cmake)
include(cmake/CcpPackageConfigHelpers.cmake)
include(cmake/CcpBuildConfigurations.cmake)

ccp_add_library(geo2 SHARED)

find_package(Python3 COMPONENTS Development Interpreter REQUIRED)
find_package(carbon-core CONFIG REQUIRED)
find_package(carbon-math CONFIG REQUIRED)

target_link_libraries(geo2 PRIVATE Python3::Python CcpMath CcpCore)

file(GLOB SRC_FILES "src/*")
target_sources(geo2 PRIVATE
    ${SRC_FILES}
)

set_target_properties(geo2 PROPERTIES OUTPUT_NAME "_geo2")


if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    option(BUILD_TESTING "Build tests. Enabled by default." ON)
    if(BUILD_TESTING)
        include(CTest)

        # discover python tests ... this may be a little bit brittle since we're working around the missing `_blueexposuretest`
        # binary. As in, test discovery happens during the configure stage at which the binary does not exist yet.
        # Discovery works with the standard python interpreter because it isn't running anything, it just needs to find all
        # the defined test cases. Therefore, just use that one, even though it cannot run the discovered test cases.
        set(ENV{PYTHONPATH} ${CMAKE_CURRENT_SOURCE_DIR}/python)
        execute_process(COMMAND ${Python3_EXECUTABLE} discover.py
                OUTPUT_VARIABLE PYTHON_TESTS_STR
                OUTPUT_STRIP_TRAILING_WHITESPACE
                ERROR_STRIP_TRAILING_WHITESPACE
                RESULT_VARIABLE PYTHON_TEST_DISCOVERY_RETVAL
                ERROR_VARIABLE PYTHON_TEST_DISCOVERY_ERRORS
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/python
        )

        if(${PYTHON_TEST_DISCOVERY_RETVAL})
            message(FATAL_ERROR "Error encountered during test discovery:\n=== stderr: ===\n${PYTHON_TEST_DISCOVERY_ERRORS}\n=== stdout: ===\n${PYTHON_TESTS_STR}")
        endif()

        separate_arguments(PYTHON_TESTS UNIX_COMMAND ${PYTHON_TESTS_STR})
        message(STATUS ${Python3_EXECUTABLE})
        foreach(PYTHON_TEST ${PYTHON_TESTS})
            message(STATUS "Found test ${PYTHON_TEST}")
            add_test(NAME ${PYTHON_TEST}
                    COMMAND ${Python3_EXECUTABLE} -m unittest -v ${PYTHON_TEST}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/python
            )
            if(WIN32)
                set_tests_properties(${PYTHON_TEST} PROPERTIES ENVIRONMENT  "PYTHONPATH=$<TARGET_FILE_DIR:geo2>\;${CMAKE_CURRENT_SOURCE_DIR}/python\;${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin\;${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib\;${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/python\;${Python3_STDLIB};BUILDFLAVOR=$<LOWER_CASE:$<CONFIG>>")
            elseif (APPLE OR LINUX)
                set_tests_properties(${PYTHON_TEST} PROPERTIES ENVIRONMENT "PYTHONPATH=$<TARGET_FILE_DIR:geo2>:${CMAKE_CURRENT_SOURCE_DIR}/python:${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin:${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib:${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/python3.12/lib-dynload:${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/python:${Python3_STDLIB};BUILDFLAVOR=$<LOWER_CASE:$<CONFIG>>")
            else()
                message(FATAL_ERROR "Unsupported platform")
            endif ()
        endforeach()
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
