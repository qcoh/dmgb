set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source")

set(SOURCES
	${INCLUDE_DIR}/mmu.h

	${SOURCE_DIR}/mmu.cpp
)

add_library(mmu STATIC ${SOURCES})

target_include_directories(
	mmu
	PUBLIC "${INCLUDE_DIR}"
)
