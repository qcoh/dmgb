set(SOURCES
	source/word.cpp
	source/rw_ref.cpp
)

enable_unity_build(utl SOURCES)

add_library(utl STATIC
	${SOURCES})

target_include_directories(utl PUBLIC
	include)

