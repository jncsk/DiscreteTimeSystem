# Specify target version
cmake_minimum_required(VERSION 3.20)

# Specify project name and the used language in the program
project(DiscreteTimeSystem LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify the name of execute file and source files compositing the exe file
add_executable(DiscreteTimeSystemRunner
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemRunner/app/src/main.cpp
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemRunner/app/src/runner_io_json.cpp
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemRunner/core/src/runner_error_map.cpp
    
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemLib/app/src/app_motor/app_motor.c
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemLib/control/src/state_space_discrete.c
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemLib/core/src/core_error.c
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemLib/control/src/state_space_c2d.c
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemLib/numerics/src/integrators/rk4.c
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemLib/numerics/src/linalg/matrix_solve.c
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemLib/numerics/src/linalg/matrix_norm.c
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemLib/numerics/src/math_utils.c
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemLib/core/src/core_matrix.c
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemLib/numerics/src/linalg/matrix_exp.c
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemLib/numerics/src/linalg/matrix_ops.c
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemLib/control/src/state_space.c
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemLib/app/src/main.c
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemLib/numerics/src/bit_utils.c
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemLib/numerics/src/pade/pade.c
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemLib/numerics/src/pade/pade_exp_coeffs.c
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemLib/numerics/src/pade/pade_scaling.c
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
    
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemLib/core/include
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemLib/control/include
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemLib/numerics/include/pade
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemLib/numerics/include/linalg
    ${CMAKE_SOURCE_DIR}/DiscreteTimeSystemLib/control/include
)

# TODO: 必要ならリンク（pthread, m, openblas等）
# target_link_libraries(DiscreteTimeSystemRunner PRIVATE ...)
