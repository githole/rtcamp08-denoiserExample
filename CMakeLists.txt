cmake_minimum_required (VERSION 3.8)

project ("DenoiserExample")

add_executable (DenoiserExample "main.cpp"    "utility.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET DenoiserExample PROPERTY CXX_STANDARD 20)
endif()

# For OpenMP
find_package(OpenMP REQUIRED)
if(OpenMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()
