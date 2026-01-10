cmake_minimum_required(VERSION 3.20)
project(GymBot CXX)
set(CMAKE_CXX_STANDARD 20)

find_package(GTest REQUIRED)
enable_testing()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES  
  tests/domain/entities/*.cc
  tests/domain/repositories/*.cc
  src/domain/repositories/*.cc
  src/service/*.cc
  tests/service/*.cc)
add_executable(GymBotTests  ${SOURCES})

target_link_libraries(GymBotTests 
    GTest::gtest 
    GTest::gtest_main 
    GTest::gmock
    sqlite3
)

target_include_directories(GymBotTests 
    PRIVATE 
    ${CMAKE_SOURCE_DIR}/src 
    ${CMAKE_SOURCE_DIR}/tests  
)

file(GLOB_RECURSE ALL_SOURCE_FILES *.cc *.h)
add_custom_target(format COMMAND clang-format -i ${ALL_SOURCE_FILES})
add_test(NAME GymBotTests COMMAND GymBotTests)
