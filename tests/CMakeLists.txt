
add_test(NAME PythonTests
        COMMAND ${EXEFILE_INTERPRETER} /inherit /buildflavor=$<LOWER_CASE:$<CONFIG>> /py -m unittest discover -v ${CMAKE_CURRENT_SOURCE_DIR}/python
        COMMAND_EXPAND_LISTS
)

if (WIN32)
    set_tests_properties(PythonTests PROPERTIES ENVIRONMENT "PYTHONPATH=${PYTHONPATH}\;${CMAKE_CURRENT_SOURCE_DIR}/../python;BUILDFLAVOR=$<LOWER_CASE:$<CONFIG>>")
else ()
    set_tests_properties(PythonTests PROPERTIES ENVIRONMENT "PYTHONPATH=${PYTHONPATH}:${CMAKE_CURRENT_SOURCE_DIR}/../python;BUILDFLAVOR=$<LOWER_CASE:$<CONFIG>>")
endif ()