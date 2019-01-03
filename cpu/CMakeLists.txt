set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source")

set(SOURCES
	${INCLUDE_DIR}/cpu.h
	${INCLUDE_DIR}/rw_ref.h
	${INCLUDE_DIR}/bitref.h
	${INCLUDE_DIR}/cpu_mmu.h

	${SOURCE_DIR}/cpu.cpp
)

add_library(cpu STATIC ${SOURCES})

target_include_directories(
	cpu
	PUBLIC "${INCLUDE_DIR}"
)
