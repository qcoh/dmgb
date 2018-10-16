cmake_minimum_required(VERSION 3.5)

project(dmgb VERSION 0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-Wall -Wextra -pedantic)

# hmpf
include_directories(
	${PROJECT_SOURCE_DIR}/cpu/include
	${PROJECT_SOURCE_DIR}/mmu/include
	${PROJECT_SOURCE_DIR}/utl/include
)

add_subdirectory(cpu)
add_subdirectory(mmu)
add_subdirectory(utl)

enable_testing()
add_subdirectory(dmgb_test)

add_executable(${PROJECT_NAME}
	dmgb.cpp
)

target_link_libraries(${PROJECT_NAME}
	cpu
	mmu
	utl
)
