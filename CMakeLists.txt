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

add_subdirectory(external/rapidcheck)
add_subdirectory(external/catch2)

add_executable(${PROJECT_NAME}
	dmgb.cpp
)

target_link_libraries(${PROJECT_NAME}
	cpu
	mmu
	utl
)

add_executable(${PROJECT_NAME}_test
	dmgb_test.cpp
)

target_link_libraries(${PROJECT_NAME}_test
	cpu
	mmu
	utl
	rapidcheck
	Catch2::Catch2
)
