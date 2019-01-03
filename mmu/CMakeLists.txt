set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source")

set(SOURCES
	${INCLUDE_DIR}/mmu.h
	${INCLUDE_DIR}/bios.h
	${INCLUDE_DIR}/rom_only.h

	${SOURCE_DIR}/mmu.cpp
	${SOURCE_DIR}/bios.cpp
	${SOURCE_DIR}/rom_only.cpp
)

add_library(mmu STATIC ${SOURCES})

target_include_directories(
	mmu
	PUBLIC "${INCLUDE_DIR}"
)

target_link_libraries(mmu CONAN_PKG::gsl_microsoft)
