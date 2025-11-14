cmake_minimum_required(VERSION 3.10)

project(tracker-qa)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)

# Specify the C++ source and header files
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add source files
set(SOURCE_FILES
    src/tracker_qa.cxx 
)   

# Create the executable from the source files
add_executable(tracker-qa ${SOURCE_FILES})

