cmake_minimum_required(VERSION 3.16)

project(djm_interpreter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
   # The interpreter uses exceptions for runtime errors and control-flow signals.
   add_compile_options(/EHsc)
endif()

########################################################################
add_library(djm_core
   src/syntax/Node.cpp

   src/source/StringSource.cpp
   src/source/FileSource.cpp

   src/lexer/Token.cpp
   src/lexer/Lexer.cpp
   src/lexer/CharBasicLib.cpp

   src/parser/Parser.cpp

   src/interpreter/RuntimeValue.cpp
   src/interpreter/Environment.cpp
   src/interpreter/Interpreter.cpp

   src/diagnostics/Error.cpp
   src/diagnostics/ErrorHandler.cpp

)

target_include_directories(djm_core PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)

########################################################################

add_executable(djm src/main.cpp)
target_link_libraries(djm PRIVATE djm_core)

########################################################################

include(FetchContent) 
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest) 

enable_testing() 
include(GoogleTest) 

# Pomocnicza zmienna z nagłówkami dla testów
set(TEST_INCLUDES 
    ${CMAKE_SOURCE_DIR}/include 
    ${CMAKE_SOURCE_DIR}/tests
)

########################################################################

add_executable(lexer_keywords_tests tests/TestUtils.cpp tests/lexer_keywords_tests.cpp) 
target_link_libraries(lexer_keywords_tests PRIVATE djm_core gtest_main) 
target_include_directories(lexer_keywords_tests PRIVATE ${TEST_INCLUDES}) 
gtest_discover_tests(lexer_keywords_tests) 

add_executable(lexer_strings_tests tests/TestUtils.cpp tests/lexer_strings_tests.cpp) 
target_link_libraries(lexer_strings_tests PRIVATE djm_core gtest_main) 
target_include_directories(lexer_strings_tests PRIVATE ${TEST_INCLUDES}) 
gtest_discover_tests(lexer_strings_tests) 

add_executable(lexer_number_tests tests/TestUtils.cpp tests/lexer_number_tests.cpp) 
target_link_libraries(lexer_number_tests PRIVATE djm_core gtest_main) 
target_include_directories(lexer_number_tests PRIVATE ${TEST_INCLUDES}) 
gtest_discover_tests(lexer_number_tests) 

add_executable(lexer_file_tests tests/TestUtils.cpp tests/lexer_file_tests.cpp) 
target_link_libraries(lexer_file_tests PRIVATE djm_core gtest_main) 
target_include_directories(lexer_file_tests PRIVATE ${TEST_INCLUDES}) 
gtest_discover_tests(lexer_file_tests) 

add_executable(lexer_functions_program_test tests/TestUtils.cpp tests/lexer_functions_program_test.cpp) 
target_link_libraries(lexer_functions_program_test PRIVATE djm_core gtest_main) 
target_include_directories(lexer_functions_program_test PRIVATE ${TEST_INCLUDES}) 
gtest_discover_tests(lexer_functions_program_test WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(lexer_char_tests tests/TestUtils.cpp tests/lexer_char_tests.cpp) 
target_link_libraries(lexer_char_tests PRIVATE djm_core gtest_main) 
target_include_directories(lexer_char_tests PRIVATE ${TEST_INCLUDES}) 
gtest_discover_tests(lexer_char_tests) 

add_executable(token_consistency_tests tests/TestUtils.cpp tests/token_consistency_tests.cpp) 
target_link_libraries(token_consistency_tests PRIVATE djm_core gtest_main) 
target_include_directories(token_consistency_tests PRIVATE ${TEST_INCLUDES}) 
gtest_discover_tests(token_consistency_tests) 

add_executable(runtime_value_tests tests/runtime_value_tests.cpp)
target_link_libraries(runtime_value_tests PRIVATE djm_core gtest_main)
target_include_directories(runtime_value_tests PRIVATE ${TEST_INCLUDES})
gtest_discover_tests(runtime_value_tests)

add_executable(environment_tests tests/environment_tests.cpp)
target_link_libraries(environment_tests PRIVATE djm_core gtest_main)
target_include_directories(environment_tests PRIVATE ${TEST_INCLUDES})
gtest_discover_tests(environment_tests)

add_executable(interpreter_import_tests tests/interpreter_import_tests.cpp)
target_link_libraries(interpreter_import_tests PRIVATE djm_core gtest_main)
target_include_directories(interpreter_import_tests PRIVATE ${TEST_INCLUDES})
gtest_discover_tests(interpreter_import_tests)

function(add_interpreter_test target source)
   add_executable(${target} tests/InterpreterTestUtils.cpp ${source})
   target_link_libraries(${target} PRIVATE djm_core gtest_main)
   target_include_directories(${target} PRIVATE ${TEST_INCLUDES})
   gtest_discover_tests(${target})
endfunction()

add_interpreter_test(interpreter_program_tests tests/interpreter_program_tests.cpp)
add_interpreter_test(interpreter_expression_tests tests/interpreter_expression_tests.cpp)
add_interpreter_test(interpreter_environment_tests tests/interpreter_environment_tests.cpp)
add_interpreter_test(interpreter_control_flow_tests tests/interpreter_control_flow_tests.cpp)
add_interpreter_test(interpreter_function_tests tests/interpreter_function_tests.cpp)
add_interpreter_test(interpreter_list_operator_tests tests/interpreter_list_operator_tests.cpp)

add_executable(parser_simple_tests tests/TestUtils.cpp tests/parser_simple_tests.cpp) 
target_link_libraries(parser_simple_tests PRIVATE djm_core gtest_main) 
target_include_directories(parser_simple_tests PRIVATE ${TEST_INCLUDES}) 
gtest_discover_tests(parser_simple_tests) 

add_executable(parse_program_tests tests/TestUtils.cpp tests/parse_program_tests.cpp) 
target_link_libraries(parse_program_tests PRIVATE djm_core gtest_main) 
target_include_directories(parse_program_tests PRIVATE ${TEST_INCLUDES}) 
gtest_discover_tests(parse_program_tests)

add_executable(parser_unit_tests tests/TestUtils.cpp tests/parser_unit_tests.cpp) 
target_link_libraries(parser_unit_tests PRIVATE djm_core gtest_main) 
target_include_directories(parser_unit_tests PRIVATE ${TEST_INCLUDES}) 
gtest_discover_tests(parser_unit_tests)

add_executable(parser_complex_expression_tests tests/TestUtils.cpp tests/parser_complex_expression_tests.cpp) 
target_link_libraries(parser_complex_expression_tests PRIVATE djm_core gtest_main) 
target_include_directories(parser_complex_expression_tests PRIVATE ${TEST_INCLUDES}) 
gtest_discover_tests(parser_complex_expression_tests)


#[===[
komentarz
wieloliniowy
]===]
