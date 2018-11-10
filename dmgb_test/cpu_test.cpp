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

		rc::PROPERTY("adding to a",
		[&c, &m](const u8 randa, const u8 randv, const bool randcf) {
			c.a = randa;
			c.b = randv;
			c.c = randv;
			c.d = randv;
			c.e = randv;
			c.h = randv;
			c.l = randv;
			m[c.hl] = randv;
			c.cf = randcf;

			m[c.pc] = *rc::gen::inRange(0x80, 0x87);

			cpu::step(c, m);

			RC_ASSERT(c.a == static_cast<u8>(randa + randv));
		});
	}
}

SCENARIO("adc", "[cpu]") {
	GIVEN("cpu and mmu") {
		cpu::cpu c{};
		u8 m[0x10000] = {0};

		rc::PROPERTY("adding to a with carry",
		[&c, &m](const u8 randa, const u8 randv, const bool randcf) {
			c.a = randa;
			c.b = randv;
			c.c = randv;
			c.d = randv;
			c.e = randv;
			c.h = randv;
			c.l = randv;
			m[c.hl] = randv;
			c.cf = randcf;

			m[c.pc] = *rc::gen::inRange(0x88, 0x8f);

			cpu::step(c, m);

			RC_ASSERT(c.a == static_cast<u8>(randa + randv + randcf));
		});
	}
}

SCENARIO("sub", "[cpu]") {
	GIVEN("cpu and mmu") {
		cpu::cpu c{};
		u8 m[0x10000] = {0};

		rc::PROPERTY("subtracting from a",
		[&c, &m](const u8 randa, const u8 randv) {
			c.a = randa;
			c.b = randv;
			c.c = randv;
			c.d = randv;
			c.e = randv;
			c.h = randv;
			c.l = randv;
			m[c.hl] = randv;

			m[c.pc] = *rc::gen::inRange(0x90, 0x97);

			cpu::step(c, m);

			RC_ASSERT(c.a == static_cast<u8>(randa - randv));
			RC_ASSERT(c.nf == true);
		});
	}
}
