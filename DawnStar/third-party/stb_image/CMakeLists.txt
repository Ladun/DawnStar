
add_library(stb_image STATIC ${CMAKE_CURRENT_SOURCE_DIR}/stb_image.cpp)

# shape 의 include 경로 지정
target_include_directories(stb_image PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# shape 의 컴파일 옵션 지정
target_compile_options(stb_image PRIVATE -Wall -Werror)

# shape 를 C++ 17 로 컴파일
target_compile_features(stb_image PRIVATE cxx_std_17)