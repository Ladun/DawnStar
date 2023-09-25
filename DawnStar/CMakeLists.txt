cmake_minimum_required(VERSION 3.13)
 
set(PROJECT_NAME DawnStar)
 
# set the project name and version
project(${PROJECT_NAME}
        VERSION 1.0
        LANGUAGES CXX    )

# find source
file(GLOB_RECURSE SOURCES
     ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/*.cpp)

# Add libraries
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third-party/spdlog)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third-party/glm)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third-party/glfw)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third-party/glad)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third-party/stb_image)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third-party/imgui)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third-party/optick)

# make exe file
add_library(${PROJECT_NAME} STATIC 
            ${SOURCES}
)
target_include_directories(${PROJECT_NAME} PUBLIC 
                           ${CMAKE_CURRENT_SOURCE_DIR}
                           ${CMAKE_CURRENT_SOURCE_DIR}/third-party/entt/include)

# Linking
target_link_libraries(${PROJECT_NAME}
                      spdlog
                      glm
                      glfw
                      glad
                      stb_image
                      imgui
                      OptickCore)

if(COMMAND target_precompile_headers)
    target_precompile_headers(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/DawnStar/dspch.hpp)
    message("Use precompiled headers")
endif()

# compile option for c17
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)