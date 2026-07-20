# ====================================================================
# Configuração do Projeto
# ====================================================================
cmake_minimum_required(VERSION 3.16)
project(c_json VERSION 2.0.0 LANGUAGES C CXX)

# ====================================================================
# Opções de Build
# ====================================================================
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

option(ENABLE_COVERAGE "Enable code coverage reporting" OFF)
option(BUILD_TESTS "Build the unit tests" ON)
option(ENABLE_FUZZING "Build libFuzzer targets (requires Clang)" OFF)

if(ANDROID)
    set(BUILD_TESTS OFF CACHE BOOL "Disable tests on Android cross-compile" FORCE)
endif()

if(ENABLE_COVERAGE)
    if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
        message(STATUS "Coverage enabled. Adding --coverage flag.")
        add_compile_options(--coverage -O0 -g)
        add_link_options(--coverage)
    else()
        message(WARNING "Code coverage is only supported for GCC/Clang.")
    endif()
endif()

# ====================================================================
# Arquivos do Projeto
# ====================================================================
set(LIB_SOURCES
    src/cJSON.c
    src/c_json.c
    src/c_json_list.c
    src/c_json_mem.c
)

# ====================================================================
# Definição da Biblioteca
# ====================================================================
add_library(${PROJECT_NAME} SHARED ${LIB_SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "c_json")

target_compile_features(${PROJECT_NAME} PUBLIC c_std_11)
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<INSTALL_INTERFACE:include>
)

message(STATUS "Project Name: ${PROJECT_NAME}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Target System: ${CMAKE_SYSTEM_NAME}")

# ====================================================================
# Configuração de Testes (Google Test + CTest)
# ====================================================================
enable_testing()

if(BUILD_TESTS)
    message(STATUS "Building with tests ENABLED.")

    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    set(TEST_SOURCES
        test/test_parser.cpp
        test/test_add.cpp
        test/test_get.cpp
        test/test_edge_cases.cpp
        test/test_manual.cpp
        test/test_coverage.cpp
        test/test_error_paths.cpp
        test/test_mem.cpp
    )

    add_executable(c_json_tests ${TEST_SOURCES})
    set_target_properties(c_json_tests PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

    target_include_directories(c_json_tests
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/inc
            ${CMAKE_CURRENT_SOURCE_DIR}/test
    )

    target_link_libraries(c_json_tests PRIVATE ${PROJECT_NAME} GTest::gtest_main)
    add_test(NAME c_json_tests COMMAND c_json_tests)
else()
    message(STATUS "Building with tests DISABLED.")
endif()

if(ENABLE_FUZZING)
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        message(FATAL_ERROR "ENABLE_FUZZING requires Clang")
    endif()
    add_executable(fuzz_parser fuzz/fuzz_parser.c)
    target_link_libraries(fuzz_parser PRIVATE ${PROJECT_NAME})
    target_compile_options(fuzz_parser PRIVATE -fsanitize=fuzzer,address,undefined -fno-omit-frame-pointer)
    target_link_options(fuzz_parser PRIVATE -fsanitize=fuzzer,address,undefined)
endif()

# ====================================================================
# Regras de Instalação
# ====================================================================
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-targets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(DIRECTORY inc/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

message(STATUS "CMake configuration finished successfully.")
