set(SOURCES
	source/mmu.cpp
	source/bios.cpp
	source/rom_only.cpp
)

enable_unity_build(mmu SOURCES)

add_library(mmu STATIC
	${SOURCES})

target_include_directories(mmu PUBLIC
	include)

target_link_libraries(mmu PRIVATE 
	CONAN_PKG::gsl_microsoft
	cpu
	utl)
