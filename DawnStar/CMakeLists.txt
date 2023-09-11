cmake_minimum_required(VERSION 3.10)
 
set(PROJECT_NAME DawnStar)
 
# set the project name and version
project(${PROJECT_NAME}
        VERSION 1.0
        LANGUAGES CXX    )

find_package(imgui REQUIRED)


# find source and header
file(GLOB_RECURSE HEADERS
     ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/**/*.hpp)

file(GLOB_RECURSE SOURCES
     ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/**/*.cpp)

# make exe file
add_library(${PROJECT_NAME} STATIC 
            ${SOURCES}
)

# Linking


# compile option for c17
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)