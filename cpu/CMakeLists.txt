set(SOURCES
	source/cpu.cpp)

enable_unity_build(cpu SOURCES)

add_library(cpu STATIC
	${SOURCES})

target_include_directories(cpu PUBLIC
	include)

target_link_libraries(cpu PRIVATE
	mmu
	utl)
