#include "catch.hpp"
#include "types.h"

SCENARIO("Array reference check at compile time", "[utl]") {
	GIVEN("A u8 array of length 0x1000") {
		u8 arr[0x10000] = {0};

		auto fn = [](mmu_ref mr){ mr[0] = 1; };
		WHEN("passing to a function accepting a mmu_ref") {
			fn(arr);

			THEN("this compiles") {
				REQUIRE(true);
			}
		}
		WHEN("passing to a function which modifies the mmu_ref") {
			fn(arr);

			THEN("the original array is modified") {
				REQUIRE(arr[0] == 1);
			}
		}
	}

#if 0
	GIVEN("A u8 array of length 0xffff") {
		u8 arr[0xffff] = {0};

		auto fn = [](mmu_ref mr){ mr[0] = 1; };
		WHEN("passing to a function accepting a mmu_ref") {
			fn(arr);

			THEN("this does not compile!") {
				REQUIRE(true);
			}
		}
	}
#endif
}
