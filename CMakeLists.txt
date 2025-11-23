# Set the minimum version of CMake that's required
cmake_minimum_required(VERSION 3.15)

# Project details
project(tracker-qa 
    VERSION 0.1
    DESCRIPTION "QA framework for the LDMX detector."
    LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Specify where to find Rogue if not already defined
if (DEFINED ENV{ROGUE_DIR})
    set(Rogue_DIR $ENV{ROGUE_DIR}/lib)
elseif (NOT DEFINED Rogue_DIR)
    set(Rogue_DIR ${CMAKE_PREFIX_PATH}/lib)
endif()

# Find Rogue
find_package(Rogue CONFIG REQUIRED)

# Check if Rogue was found
if (NOT Rogue_FOUND)
    message(FATAL_ERROR "Failed to find required dependency Rogue.")
endif()

# If Rogue::Rogue was not created by find_package, create it manually
if (NOT TARGET Rogue::Rogue)
    add_library(Rogue::Rogue INTERFACE IMPORTED)
    set_target_properties(Rogue::Rogue PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${ROGUE_INCLUDE_DIRS}"
        INTERFACE_LINK_LIBRARIES "${ROGUE_LIBRARIES}")
endif()

# Build the QA library
add_library(qa ${PROJECT_SOURCE_DIR}/src/qa/CsvWriter.cxx)

# Headers needed by the QA library
target_include_directories(qa PRIVATE ${PROJECT_SOURCE_DIR}/include/)

# Link the Rogue library to the QA library
target_link_libraries(qa PRIVATE Rogue::Rogue)

# Create the executable 
add_executable(tracker-qa src/tracker_qa.cxx)

# Headers needed by tracker-qa
target_include_directories(tracker-qa PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Link the Rogue library to the tracker-qa target
target_link_libraries(tracker-qa PRIVATE Rogue::Rogue qa)

# Enforce cxx17
target_compile_features(tracker-qa PUBLIC cxx_std_17)

# Installation
install(TARGETS tracker-qa 
    RUNTIME DESTINATION bin)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include 
    DESTINATION include
    FILES_MATCHING PATTERN "*.h")
