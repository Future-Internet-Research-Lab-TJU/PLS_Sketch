
add_library(Models STATIC
    Data/Data.cpp
    CardinalityEstimator/HyperLogLog.cpp
    CardinalityEstimator/LinearCounting.cpp
    PLSSketch/PLSSketch.cpp
    PLSSketch/PLSSketchTest.cpp
)

target_include_directories(Models PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Data
    ${CMAKE_CURRENT_SOURCE_DIR}/CardinalityEstimator
    ${CMAKE_CURRENT_SOURCE_DIR}/PLSSketch
)

target_link_libraries(Models PRIVATE
    Utils
    stdc++exp
)