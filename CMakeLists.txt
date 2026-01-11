cmake_minimum_required(VERSION 3.20)
project(GymBot CXX)
set(CMAKE_CXX_STANDARD 23)

find_package(GTest REQUIRED)
enable_testing()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(Boost 1.83 REQUIRED COMPONENTS json)

file(GLOB_RECURSE SOURCES  
  tests/domain/entities/*.cc
  tests/domain/repositories/*.cc
  src/domain/repositories/*.cc
  src/service/*.cc
  tests/service/*.cc)
add_executable(GymBotTests  ${SOURCES})

target_link_libraries(GymBotTests 
    PRIVATE
        GTest::gtest 
        GTest::gtest_main 
        GTest::gmock
        sqlite3
        Boost::json 
)

target_include_directories(GymBotTests 
    PRIVATE 
    ${CMAKE_SOURCE_DIR}/src 
    ${CMAKE_SOURCE_DIR}/tests  
)


file(GLOB_RECURSE SOURCES  
  src/domain/repositories/*.cc
  src/service/*.cc)
find_package(TgBot REQUIRED)
add_executable(GymBot ${SOURCES} src/main.cc)
target_include_directories(GymBot 
    PRIVATE 
    ${CMAKE_SOURCE_DIR}/src 
)
target_link_libraries(GymBot sqlite3 TgBot::TgBot Boost::json 
)

set(PROJECT_SOURCE_DIR "${CMAKE_SOURCE_DIR}")

target_compile_definitions(GymBot PRIVATE 
  PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}"
  SCHEMA_PATH="${PROJECT_SOURCE_DIR}/resources/schema.sql")

target_compile_definitions(GymBotTests PRIVATE 
  PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}"
  SCHEMA_PATH="${PROJECT_SOURCE_DIR}/resources/schema.sql")
  
option(ENABLE_COVERAGE "Enable code coverage" OFF)
if(ENABLE_COVERAGE)
    target_compile_options(GymBotTests PRIVATE
        -fprofile-arcs
        -ftest-coverage
    )
    target_link_libraries(GymBotTests PRIVATE
        gcov
    )
endif()

file(GLOB_RECURSE ALL_SOURCE_FILES *.cc *.h)
add_custom_target(format COMMAND clang-format -i ${ALL_SOURCE_FILES})
add_test(NAME GymBotTests COMMAND GymBotTests)

if(ENABLE_COVERAGE)
    find_program(LCOV_PATH lcov)
    find_program(GENHTML_PATH genhtml)
    
    if(LCOV_PATH AND GENHTML_PATH)
        add_custom_target(coverage
            COMMAND ${LCOV_PATH} --directory . --zerocounters
            COMMAND $<TARGET_FILE:GymBotTests>
            COMMAND ${LCOV_PATH} --directory . --capture --output-file coverage.info
            COMMAND ${LCOV_PATH} --remove coverage.info '/usr/*' '*/tests/*' '*/googletest/*' --output-file coverage.filtered.info
            COMMAND ${GENHTML_PATH} coverage.filtered.info --output-directory coverage_report
            COMMAND ${CMAKE_COMMAND} -E echo "Coverage report generated in coverage_report/index.html"
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS GymBotTests
        )
    endif()
endif()