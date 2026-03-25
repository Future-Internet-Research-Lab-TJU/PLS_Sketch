
add_library(Utils STATIC
    source/utils.cpp
    source/murmur.cpp
    source/reader.cpp
    source/PcapReader.cpp
)

target_include_directories(Utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)