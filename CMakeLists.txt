cmake_minimum_required(VERSION 3.16)

project(djm_interpreter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

########################################################################
add_library(djm_core
   src/source/StringSource.cpp
   src/source/FileSource.cpp

   src/lexer/Token.cpp
   src/lexer/Lexer.cpp
   src/lexer/CharBasicLib.cpp

)

target_include_directories(djm_core PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)

########################################################################

add_executable(djm src/main.cpp)
target_link_libraries(djm PRIVATE djm_core)

########################################################################

enable_testing()

add_executable(lexer_keywords_tests
   tests/TestUtils.cpp
   tests/lexer_keywords_tests.cpp
)

target_link_libraries(lexer_keywords_tests PRIVATE djm_core)

target_include_directories(lexer_keywords_tests PRIVATE
   ${CMAKE_SOURCE_DIR}/include
   ${CMAKE_SOURCE_DIR}/tests
)

add_test(NAME lexer_keywords_tests COMMAND lexer_keywords_tests)

########################################################################

add_executable(lexer_strings_tests
   tests/TestUtils.cpp
   tests/lexer_strings_tests.cpp
)

target_link_libraries(lexer_strings_tests PRIVATE djm_core)

target_include_directories(lexer_strings_tests PRIVATE
   ${CMAKE_SOURCE_DIR}/include
   ${CMAKE_SOURCE_DIR}/tests
)

add_test(NAME lexer_strings_tests COMMAND lexer_strings_tests)

########################################################################

add_executable(lexer_file_tests
   tests/TestUtils.cpp
   tests/lexer_file_tests.cpp
)

target_link_libraries(lexer_file_tests PRIVATE djm_core)

target_include_directories(lexer_file_tests PRIVATE
   ${CMAKE_SOURCE_DIR}/include
   ${CMAKE_SOURCE_DIR}/tests
)

add_test(NAME lexer_file_tests COMMAND lexer_file_tests)

########################################################################
