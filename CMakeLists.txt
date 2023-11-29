cmake_minimum_required(VERSION 3.26)
project(proyecto3_reconstruccion_triangular)

set(CMAKE_CXX_STANDARD 17)

add_executable(proyecto3_reconstruccion_triangular
        main.cpp
        glad.c
        Objeto.cpp)

target_link_libraries(proyecto3_reconstruccion_triangular -lOpenGL32 -lglfw3)
