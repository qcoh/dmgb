set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source")

set(SOURCES
	${INCLUDE_DIR}/types.h
	${INCLUDE_DIR}/word.h

	${SOURCE_DIR}/word.cpp
)

add_library(utl STATIC ${SOURCES})

target_include_directories(
	utl
	PUBLIC "${INCLUDE_DIR}"
)

