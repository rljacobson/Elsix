cmake_minimum_required(VERSION 3.13)

#### Generic ####
# Show AND log compilation commands.
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
# Use C++17.
set(CMAKE_CXX_STANDARD 17)

project(Elsix)
set(PROJECT_DESCRIPTION "An implementation of the Bell Telephone Laboratories' Low-Level Linked
List Language L6.")

# Detect of the compiler supports computed gotos. CURRENTLY UNUSED.
#   clang++-5.0 or newer, GCC 6 or newer.
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # Requires at least gcc 6
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 6)
        add_compile_definitions(USE_COMPUTED_GOTO=1)
    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # Requires at least clang 5
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 5)
        add_compile_definitions(USE_COMPUTED_GOTO=1)
    endif()
endif()

# Dependencies
if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
else()
    message(WARNING "The file conanbuildinfo.cmake doesn't exist, you have to run conan install
    first.")
endif()

set(ELSIX_SOURCES_HEADERS
        src/parser.hpp
        src/astnode.hpp
        src/visitor.hpp
        src/error.hpp
        src/operators.hpp
        src/stringutilities.hpp
        src/reservedwords.hpp
        src/tokenstream.hpp
        src/sourcefile.hpp
        src/location.hpp)
set(ELSIX_SOURCES ${ELSIX_SOURCES_HEADERS}
        src/parser.cpp
        src/astnode.cpp
        src/visitor.cpp
        src/error.cpp
        src/stringutilities.cpp
        src/reservedwords.cpp
        src/tokenstream.cpp
        src/sourcefile.cpp)

add_executable(Elsix ${ELSIX_SOURCES})
target_include_directories(Elsix PRIVATE src) # ${CONAN_INCLUDE_DIRS}
target_link_libraries(Elsix PRIVATE ${CONAN_LIBS})


# Build the documentation
add_custom_target(docs ALL
        WORKING_DIRECTORY doc
        COMMAND make html
        VERBATIM
    )
