#include <catch2/catch.hpp>

#include "cpu.h"

SCENARIO("basic lds", "[cpu]") {
	GIVEN("cpu and mmu_ref with value at register d is 12") {
		cpu::cpu c{};
		u8 m[0x10000] = {0};

		c.d = 12;

		WHEN("loading d to c") {
			m[0] = 0x4a;
			c.pc = 0;

			cpu::step(c, m);

			THEN("the value at register c is 12") {
				REQUIRE(c.c == 12);
			}
		}
	}
}

