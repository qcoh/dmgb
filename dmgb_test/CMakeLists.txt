find_package(Catch2 REQUIRED)

set(SOURCES
	bios_test.cpp
	dmgb_test.cpp
	mmu_test.cpp
	cpu_test.cpp
	bitref_test.cpp
	word_test.cpp
)

include_directories(
	include/
)

add_subdirectory(external/rapidcheck)

add_executable(${PROJECT_NAME}_test ${SOURCES})

target_link_libraries(${PROJECT_NAME}_test
	cpu
	mmu
	utl
	rapidcheck
	CONAN_PKG::catch2
)

include(CTest)
include(Catch)
catch_discover_tests(${PROJECT_NAME}_test)
