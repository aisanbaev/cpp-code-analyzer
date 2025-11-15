set(target metric_test)

add_compile_definitions(TEST_FILES_DIR="${CMAKE_CURRENT_BINARY_DIR}")

add_executable(${target}
    tests/code_lines_count.cpp
    tests/cyclomatic_complexity.cpp
    tests/naming_style.cpp 
    tests/parameters_count.cpp
)

target_link_libraries(${target}
    PRIVATE
        GTest::GTest
        GTest::Main
        metric
        function
        file
)

file(GLOB test_files "${CMAKE_CURRENT_SOURCE_DIR}/tests/files/*.py")
foreach(file IN LISTS test_files)
    get_filename_component(file_name ${file} NAME)
    configure_file(${file} "${CMAKE_CURRENT_BINARY_DIR}/${file_name}" COPYONLY)
endforeach()

add_test(NAME ${target} COMMAND ${target})

# Добавляем тесты для analyse функций
add_executable(analyse_test tests/analyse.cpp)
target_link_libraries(analyse_test PRIVATE GTest::GTest GTest::Main metric function file metric_accumulator)
add_test(NAME analyse_test COMMAND analyse_test)
