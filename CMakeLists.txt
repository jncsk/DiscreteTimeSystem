cmake_minimum_required(VERSION 3.20)
project(DiscreteTimeSystem LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(DiscreteTimeSystemRunner
  # TODO: Runner.vcxproj から拾った .cpp/.c をここに並べる
)

target_include_directories(DiscreteTimeSystemRunner PRIVATE
  # TODO: include ルートを並べる（例: ${CMAKE_SOURCE_DIR}/include など）
)

# TODO: 必要ならリンク（pthread, m, openblas等）
# target_link_libraries(DiscreteTimeSystemRunner PRIVATE ...)
