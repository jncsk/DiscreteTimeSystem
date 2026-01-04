# Specify target version
cmake_minimum_required(VERSION 3.20)

# Specify project name and the used language in the program
project(DiscreteTimeSystem LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify the name of execute file and source files compositing the exe file
add_executable(DiscreteTimeSystemRunner
    DiscreteTimeSystemRunner/app/src/main.cpp
    DiscreteTimeSystemRunner/app/src/runner_io_json.cpp
    DiscreteTimeSystemRunner/core/src/runner_error_map.cpp  
)

target_include_directories(DiscreteTimeSystemRunner PRIVATE
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemLib/app/include
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemLib/control/include
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemLib/numerics/include
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemLib/numerics/include/linalg
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemLib/core/include
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemLib/numerics/include/pade
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemLib/app/include/app_motor
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemRunner/app/include
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemRunner/core/include
)

# TODO: 必要ならリンク（pthread, m, openblas等）
# target_link_libraries(DiscreteTimeSystemRunner PRIVATE ...)
