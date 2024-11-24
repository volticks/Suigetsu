cmake_minimum_required(VERSION 3.12)
project(Suigetsu VERSION 0.01)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

SET(CMAKE_CXX_FLAGS "-std=c++2a -g -fsanitize=address")


file(GLOB SOURCES "src/*.cc")
list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/src/main.cc")
add_executable(mmu_test "${SOURCES}" "src/tests/mmu/mmu_test.cc")
add_executable(Suigetsu "${SOURCES}" "src/main.cc")

target_include_directories(Suigetsu PUBLIC "${CMAKE_SOURCE_DIRECTORY}/src/*.h")
target_include_directories(mmu_test PUBLIC "${CMAKE_SOURCE_DIRECTORY}/src/*.h")

message("${SOURCES}")
message("${CMAKE_SOURCE_DIR}")
