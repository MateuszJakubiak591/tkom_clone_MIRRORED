cmake_minimum_required(VERSION 3.16)

project(djm_interpreter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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


add_executable(djm src/main.cpp)
target_link_libraries(djm PRIVATE djm_core)


enable_testing()
add_executable(lexer_tests tests/lexer_tests.cpp)
target_link_libraries(lexer_tests PRIVATE djm_core)
add_test(NAME LexerTests COMMAND lexer_tests)