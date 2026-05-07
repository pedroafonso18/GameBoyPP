cmake_minimum_required(VERSION 3.31)
project(
  GBE
  VERSION 1.0
  LANGUAGES CXX
)

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

add_executable(GBE main.cpp)

target_link_libraries(GBE PRIVATE SDL3::SDL3)