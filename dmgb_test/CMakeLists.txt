find_package(Catch2 REQUIRED)

set(SOURCES
	bios_test.cpp
	dmgb_test.cpp
	mmu_test.cpp
	cpu_test.cpp
	bitref_test.cpp
	word_test.cpp
)

add_subdirectory(external/rapidcheck)

add_executable(dmgb_test
       	${SOURCES})

target_include_directories(dmgb_test PRIVATE
	include
)

target_link_libraries(dmgb_test PRIVATE
	cpu
	mmu
	utl
	rapidcheck
	CONAN_PKG::catch2
)

include(CTest)
include(Catch)
catch_discover_tests(dmgb_test)
