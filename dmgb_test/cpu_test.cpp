#include "test.h"

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

SCENARIO("add", "[cpu]") {
	GIVEN("a cpu and mmu") {
		cpu::cpu c{};
		u8 m[0x10000] = {0};

		c.a = 12;
		c.b = 34;

		WHEN("adding b to a") {
			m[c.pc] = 0x80;
			cpu::step(c, m);

			THEN("the sum is stored in a") {
				REQUIRE(c.a == 46);
			}
		}
	}
}

SCENARIO("adc", "[cpu]") {
	GIVEN("cpu and mmu") {
		cpu::cpu c{};
		u8 m[0x10000] = {0};

		m[c.pc] = 0x88;

		rc::PROPERTY("adding random b to a with carry",
		[&c, &m](const u8 randa, const u8 randb, const bool randcf) {
			c.a = randa;
			c.b = randb;
			c.cf = randcf;

			cpu::step(c, m);

			RC_ASSERT(c.a == static_cast<u8>(randa + randb + randcf));
		});
	}
}
