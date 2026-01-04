# Specify target version
cmake_minimum_required(VERSION 3.20)

# Specify project name and the used language in the program
project(DiscreteTimeSystem LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify the name of execute file and source files compositing the exe file
add_executable(DiscreteTimeSystemRunner
    app\src\main.cpp
    app\src\runner_io_json.cpp
    core\src\runner_error_map.cpp  
)

target_include_directories(DiscreteTimeSystemRunner PRIVATE
    $(SolutionDir)DiscreteTimeSystemLib\app\include
    $(SolutionDir)DiscreteTimeSystemLib\control\include
    $(SolutionDir)DiscreteTimeSystemLib\numerics\include
    $(SolutionDir)DiscreteTimeSystemLib\numerics\include\linalg
    $(SolutionDir)DiscreteTimeSystemLib\core\include
    $(SolutionDir)DiscreteTimeSystemLib\numerics\include\pade
    $(SolutionDir)DiscreteTimeSystemLib\app\include\app_motor
)

# TODO: 必要ならリンク（pthread, m, openblas等）
# target_link_libraries(DiscreteTimeSystemRunner PRIVATE ...)
