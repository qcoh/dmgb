cmake_minimum_required(VERSION 3.5)

project(dmgb VERSION 0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE Debug)
add_compile_options(-Wall -Wextra -pedantic -Werror)

# https://github.com/NTAP/warpcore/blob/master/CMakeLists.txt#L137
# https://github.com/decodable/called_if/blob/909929c9e697695bcfe312c38f1c09fd2d168c34/CMakeLists.txt#L6
find_program(CMAKE_CXX_CPPCHECK NAMES cppcheck)
if(CMAKE_CXX_CPPCHECK)
	list(APPEND CMAKE_CXX_CPPCHECK
		"--enable=warning"
		"--enable=style"
		"--enable=performance"
		"--enable=portability"
		"--template=\"[{severity}][{id}] {message} {callstack} (On {file}:{line})\""
		"--quiet"
		"--suppressions-list=${CMAKE_SOURCE_DIR}/cppcheck_suppressions"
	)
endif()

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

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
