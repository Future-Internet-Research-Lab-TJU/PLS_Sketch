cmake_minimum_required(VERSION 3.26)

project(Sketch)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -march=native")

add_executable(${PROJECT_NAME}
    main.cpp
)

add_subdirectory(Utils)
add_subdirectory(Models)

target_link_libraries(${PROJECT_NAME} PRIVATE
    stdc++exp
    Utils
    Models
)