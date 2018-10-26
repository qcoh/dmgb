#include "test.h"

#include <sstream>

#include "bios.h"

using namespace mmu;

SCENARIO("creating a bios object", "[bios]") {
	GIVEN("a 'file' with 0x100 bytes") {
		std::stringstream ss{};
		for (int i = 0; i < 0x100; i++) {
			ss << ' '; // 0x10
		}

		WHEN("creating a bios") {
			bios b{ss};

			THEN("the bios' contents equal the file's") {
				bool allAreEqual = true;
				for (int i = 0; i < 0x100; i++) {
					if (b[i] != ' ') {
						allAreEqual = false;
					}
				}
				REQUIRE(allAreEqual);
			}
		}
	}

	GIVEN("a 'file' with less than 0x100 bytes") {
		std::stringstream ss{};
		for (int i = 0; i < 0x40; i++) {
			ss << '\n';
		}
		
		WHEN("creating a bios") {
			THEN("an exception during construction occurs") {
				REQUIRE_THROWS(bios{ss});
			}
		}	
	}

	GIVEN("a 'file' with more than 0x100 bytes") {
		std::stringstream ss{};
		for (int i = 0; i < 0x101; i++) {
			ss << '\n';
		}
		
		WHEN("creating a bios") {
			THEN("an exception during construction occurs") {
				REQUIRE_THROWS(bios{ss});
			}
		}	
	}
}
