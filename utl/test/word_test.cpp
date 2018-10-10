#include "catch.hpp"
#include "word.h"

SCENARIO("Endianess of word", "[utl]") {
	GIVEN("A word comprising hi and lo bytes") {
		u8 hi{0U};
		u8 lo{0U};

		word w{hi, lo};

		WHEN("modifying lo") {
			lo = 0xff;

			THEN("the lsb of the word has the same value") {
				REQUIRE(static_cast<u8>(w) == 0xff);
			}
			AND_THEN("the msb of the word is unaffected") {
				REQUIRE(static_cast<u8>(w >> 8) == 0);
			}
		}
	}
}