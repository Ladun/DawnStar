cmake_minimum_required(VERSION 3.10)
project(glad
        VERSION 1.0
        LANGUAGES C)

add_library(glad STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/glad.c)

# shape 의 include 경로 지정
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# shape 의 컴파일 옵션 지정
target_compile_options(glad PRIVATE -Wall -Werror)

# shape 를 C++ 17 로 컴파일
target_compile_features(glad PRIVATE cxx_std_17)