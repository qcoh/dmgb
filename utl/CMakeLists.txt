set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source")

set(SOURCES
	${INCLUDE_DIR}/types.h
	${INCLUDE_DIR}/word.h
	${INCLUDE_DIR}/rw_ref.h
	${INCLUDE_DIR}/read_writer.h

	${SOURCE_DIR}/word.cpp
	${SOURCE_DIR}/rw_ref.cpp
)

add_library(utl STATIC ${SOURCES})

target_include_directories(
	utl
	PUBLIC "${INCLUDE_DIR}"
)

