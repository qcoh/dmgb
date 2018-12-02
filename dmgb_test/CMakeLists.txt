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

#add_subdirectory(external/catch2)
add_subdirectory(external/rapidcheck)

add_executable(${PROJECT_NAME}_test ${SOURCES})

target_link_libraries(${PROJECT_NAME}_test
	cpu
	mmu
	utl
	rapidcheck
	#Catch2::Catch2
	CONAN_PKG::catch2
)

add_test(Test dmgb_test)
