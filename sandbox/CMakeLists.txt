cmake_minimum_required(VERSION 3.10)
 
set(PROJECT_NAME SandBox-DawnStar)
 
# set the project name and version
project(${PROJECT_NAME}
        VERSION 1.0
        LANGUAGES CXX    )

# find source
file(GLOB_RECURSE SOURCES
     ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# add subprojects
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third-party/DawnStar)

# make exe file
# message("Hello", ${SOURCES})
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Linking
target_link_libraries(${PROJECT_NAME} DawnStar)

# compile option for c17
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)