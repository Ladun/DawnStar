cmake_minimum_required(VERSION 3.10)
 
set(PROJECT_NAME ldrail)
 
# set the project name and version
project(${PROJECT_NAME}
        VERSION 1.0)    
        
# opengl
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

# find source and header
file(GLOB_RECURSE HEADERS
     ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/*.hpp)

file(GLOB_RECURSE SOURCES
     ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/*.cpp)

# make exe file
add_library(${PROJECT_NAME} STATIC 
            ${SOURCES}
)

# Linking
target_link_libraries(${PROJECT_NAME} OpenGL::OpenGL OpenGL::GLU GLUT::GLUT)

# compile option for c17
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)