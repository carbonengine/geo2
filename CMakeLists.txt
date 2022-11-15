project(geo2 VERSION 1.0.0)

add_library(geo2 SHARED)
target_link_libraries(geo2 PRIVATE Python CcpMath)

target_sources(geo2 PRIVATE
    autoversion.h
    Geo2.cpp
    resource.h
    Util.cpp
    Util.h
    Vector.cpp
    Vector.h
    VectorD.cpp
    version.h
)

target_include_directories(geo2 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)

if(APPLE)
    target_link_libraries(geo2 PRIVATE CcpCore)
    set_target_properties(
        geo2
        PROPERTIES 
            PREFIX ""
            SUFFIX ".so"
    )
endif()
set_target_properties(geo2 PROPERTIES OUTPUT_NAME "_geo2")
