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

			c.pc = *rc::gen::distinctFrom(rc::gen::inRange(0, 10), c.hl);
			m[c.pc] = *rc::gen::inRange(0x80, 0x87);

			cpu::step(c, m);

			RC_ASSERT(c.a == static_cast<u8>(randa + randv));
			RC_ASSERT(c.nf == false);
			RC_ASSERT(c.zf == (c.a == 0));
			RC_ASSERT(c.hf == ((randa & 0xf) + (randv & 0xf) > 0xf));
			RC_ASSERT(c.cf == ((randa + randv) > 0xff));
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

			c.pc = *rc::gen::distinctFrom(rc::gen::inRange(0, 10), c.hl);
			m[c.pc] = *rc::gen::inRange(0x88, 0x8f);

			cpu::step(c, m);

			RC_ASSERT(c.a == static_cast<u8>(randa + randv + randcf));
			RC_ASSERT(c.nf == false);
			RC_ASSERT(c.zf == (c.a == 0));
			RC_ASSERT(c.hf == ((randa & 0xf) + (randv & 0xf) + randcf > 0xf));
			RC_ASSERT(c.cf == ((randa + randv + randcf) > 0xff));
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

			c.pc = *rc::gen::distinctFrom(rc::gen::inRange(0, 10), c.hl);
			m[c.pc] = *rc::gen::inRange(0x90, 0x97);

			cpu::step(c, m);

			RC_ASSERT(c.a == static_cast<u8>(randa - randv));
			RC_ASSERT(c.nf == true);
			RC_ASSERT(c.zf == (c.a == 0));
			RC_ASSERT(c.hf == ((randa & 0xf) < (randv & 0xf)));
			RC_ASSERT(c.cf == (randa < randv));
		});
	}
}

SCENARIO("sbc", "[cpu]") {
	GIVEN("cpu and mmu") {
		cpu::cpu c{};
		u8 m[0x10000] = {0};

		rc::PROPERTY("subtracting from a with carry",
		[&c, &m](const u8 randa, const u8 randv, const bool randcf) {
			c.a = randa;
			c.b = randv;
			c.c = randv;
			c.d = randv;
			c.e = randv;
			c.h = randv;
			c.l = randv;
			c.cf = randcf;
			m[c.hl] = randv;

			c.pc = *rc::gen::distinctFrom(rc::gen::inRange(0, 10), c.hl);
			m[c.pc] = *rc::gen::inRange(0x98, 0x9f);

			cpu::step(c, m);

			RC_ASSERT(c.a == static_cast<u8>(randa - randv - randcf));
			RC_ASSERT(c.nf == true);
			RC_ASSERT(c.zf == (c.a == 0));
			RC_ASSERT(c.hf == ((randa & 0xf) < (randv & 0xf) + randcf));
			RC_ASSERT(c.cf == (randa < randv + randcf));
		});
	}
}

SCENARIO("and", "[cpu]") {
	GIVEN("cpu and mmu") {
		cpu::cpu c{};
		u8 m[0x10000] = {0};

		rc::PROPERTY("bitwise and with a",
		[&c, &m](const u8 randa, const u8 randv) {
			c.a = randa;
			c.b = randv;
			c.c = randv;
			c.d = randv;
			c.e = randv;
			c.h = randv;
			c.l = randv;
			m[c.hl] = randv;

			c.pc = *rc::gen::distinctFrom(rc::gen::inRange(0, 10), c.hl);
			m[c.pc] = *rc::gen::inRange(0xa0, 0xa7);

			cpu::step(c, m);

			RC_ASSERT(c.a == (randa & randv));
			RC_ASSERT(c.nf == false);
			RC_ASSERT(c.hf == true);
			RC_ASSERT(c.zf == (c.a == 0));
			RC_ASSERT(c.cf == false);
		});
	}
}

SCENARIO("xor", "[cpu]") {
	GIVEN("cpu and mmu") {
		cpu::cpu c{};
		u8 m[0x10000] = {0};

		rc::PROPERTY("bitwise xor with a",
		[&c, &m](const u8 randa, const u8 randv) {
			c.a = randa;
			c.b = randv;
			c.c = randv;
			c.d = randv;
			c.e = randv;
			c.h = randv;
			c.l = randv;
			m[c.hl] = randv;

			c.pc = *rc::gen::distinctFrom(rc::gen::inRange(0, 10), c.hl);
			m[c.pc] = *rc::gen::inRange(0xa8, 0xaf);

			cpu::step(c, m);

			RC_ASSERT(c.a == (randa ^ randv));
			RC_ASSERT(c.nf == false);
			RC_ASSERT(c.hf == false);
			RC_ASSERT(c.zf == (c.a == 0));
			RC_ASSERT(c.cf == false);
		});
	}
}

SCENARIO("or", "[cpu]") {
	GIVEN("cpu and mmu") {
		cpu::cpu c{};
		u8 m[0x10000] = {0};

		rc::PROPERTY("bitwise or with a",
		[&c, &m](const u8 randa, const u8 randv) {
			c.a = randa;
			c.b = randv;
			c.c = randv;
			c.d = randv;
			c.e = randv;
			c.h = randv;
			c.l = randv;
			m[c.hl] = randv;

			c.pc = *rc::gen::distinctFrom(rc::gen::inRange(0, 10), c.hl);
			m[c.pc] = *rc::gen::inRange(0xb0, 0xb7);

			cpu::step(c, m);

			RC_ASSERT(c.a == (randa | randv));
			RC_ASSERT(c.nf == false);
			RC_ASSERT(c.hf == false);
			RC_ASSERT(c.zf == (c.a == 0));
			RC_ASSERT(c.cf == false);
		});
	}
}

SCENARIO("cp", "[cpu]") {
	GIVEN("cpu and mmu") {
		cpu::cpu c{};
		u8 m[0x10000] = {0};

		rc::PROPERTY("comparing with a",
		[&c, &m](const u8 randa, const u8 randv) {
			c.a = randa;
			c.b = randv;
			c.c = randv;
			c.d = randv;
			c.e = randv;
			c.h = randv;
			c.l = randv;
			m[c.hl] = randv;

			c.pc = *rc::gen::distinctFrom(rc::gen::inRange(0, 10), c.hl);
			m[c.pc] = *rc::gen::inRange(0xb8, 0xbf);

			cpu::step(c, m);

			RC_ASSERT(c.a == randa);
			RC_ASSERT(c.nf == true);
			RC_ASSERT(c.zf == ((randa - randv) == 0));
			RC_ASSERT(c.hf == ((randa & 0xf) < (randv & 0xf)));
			RC_ASSERT(c.cf == (randa < randv));
		});

		rc::PROPERTY("cp is the same as sub without modifying a",
		[&c, &m](const u8 randa, const u8 randv) {
			c.a = randa;
			c.b = randv;
			c.c = randv;
			c.d = randv;
			c.e = randv;
			c.h = randv;
			c.l = randv;
			m[c.hl] = randv;

			c.pc = *rc::gen::distinctFrom(rc::gen::inRange(0, 10), c.hl);
			m[c.pc] = *rc::gen::inRange(0xb8, 0xbf);

			cpu::step(c, m);

			const bool cf = c.cf;
			const bool nf = c.nf;
			const bool hf = c.hf;
			const bool zf = c.zf;

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

			RC_ASSERT(c.nf == nf);
			RC_ASSERT(c.zf == zf);
			RC_ASSERT(c.hf == hf);
			RC_ASSERT(c.cf == cf);
		});
	}
}


SCENARIO("bit", "[cpu]") {
	GIVEN("cpu and mmu") {
		cpu::cpu c{};
		u8 m[0x10000] = {0};

		rc::PROPERTY("test bit",
		[&c, &m](const u8 randv) {
			c.a = randv;
			c.b = randv;
			c.c = randv;
			c.d = randv;
			c.e = randv;
			c.h = randv;
			c.l = randv;
			m[c.hl] = randv;

			c.pc = *rc::gen::suchThat(rc::gen::inRange(0, 10),
					[&c](int x){ 
						return x != c.hl && (x+1) != c.hl;
					});

			m[c.pc] = 0xcb;
			const u8 cb = *rc::gen::inRange(0x40, 0x7f);
			m[c.pc + 1] = cb;

			cpu::step(c, m);

			const u8 Index = (cb >> 3) & 0x7;
			const u8 Mask = 1 << Index;

			RC_ASSERT(c.nf == false);
			RC_ASSERT(c.hf == true);
			RC_ASSERT(c.zf == !!(randv & Mask));
		});
	}
}

SCENARIO("res", "[cpu]") {
	GIVEN("cpu and mmu") {
		cpu::cpu c{};
		u8 m[0x10000] = {0};

		rc::PROPERTY("reset bit",
		[&c, &m](const u8 randv) {
			c.a = randv;
			c.b = randv;
			c.c = randv;
			c.d = randv;
			c.e = randv;
			c.h = randv;
			c.l = randv;
			m[c.hl] = randv;

			c.pc = *rc::gen::suchThat(rc::gen::inRange(0, 10),
					[&c](int x){ 
						return x != c.hl && (x+1) != c.hl;
					});

			m[c.pc] = 0xcb;
			const u8 cb = *rc::gen::inRange(0x80, 0xbf);
			m[c.pc + 1] = cb;

			cpu::step(c, m);

			const u8 Index = (cb >> 3) & 0x7;
			const u8 Mask = 1 << Index;

			switch (cb & 0x7) {
			case 0: RC_ASSERT((c.b & Mask) == 0); break;
			case 1: RC_ASSERT((c.c & Mask) == 0); break;
			case 2: RC_ASSERT((c.d & Mask) == 0); break;
			case 3: RC_ASSERT((c.e & Mask) == 0); break;
			case 4: RC_ASSERT((c.h & Mask) == 0); break;
			case 5: RC_ASSERT((c.l & Mask) == 0); break;
			case 6: RC_ASSERT((m[c.hl] & Mask) == 0); break;
			case 7: RC_ASSERT((c.a & Mask) == 0); break;
			}
		});
	}
}

SCENARIO("set", "[cpu]") {
	GIVEN("cpu and mmu") {
		cpu::cpu c{};
		u8 m[0x10000] = {0};

		rc::PROPERTY("set bit",
		[&c, &m](const u8 randv) {
			c.a = randv;
			c.b = randv;
			c.c = randv;
			c.d = randv;
			c.e = randv;
			c.h = randv;
			c.l = randv;
			m[c.hl] = randv;

			c.pc = *rc::gen::suchThat(rc::gen::inRange(0, 10),
					[&c](int x){ 
						return x != c.hl && (x+1) != c.hl;
					});

			m[c.pc] = 0xcb;
			const u8 cb = *rc::gen::inRange(0xc0, 0xff);
			m[c.pc + 1] = cb;

			cpu::step(c, m);

			const u8 Index = (cb >> 3) & 0x7;
			const u8 Mask = 1 << Index;

			switch (cb & 0x7) {
			case 0: RC_ASSERT((c.b & Mask) != 0); break;
			case 1: RC_ASSERT((c.c & Mask) != 0); break;
			case 2: RC_ASSERT((c.d & Mask) != 0); break;
			case 3: RC_ASSERT((c.e & Mask) != 0); break;
			case 4: RC_ASSERT((c.h & Mask) != 0); break;
			case 5: RC_ASSERT((c.l & Mask) != 0); break;
			case 6: RC_ASSERT((m[c.hl] & Mask) != 0); break;
			case 7: RC_ASSERT((c.a & Mask) != 0); break;
			}
		});
	}
}


SCENARIO("rlc", "[cpu]") {
	GIVEN("cpu and mmu") {
		cpu::cpu c{};
		u8 m[0x10000] = {0};

		rc::PROPERTY("rlc",
		[&c, &m](const u8 randv) {
			c.a = randv;
			c.b = randv;
			c.c = randv;
			c.d = randv;
			c.e = randv;
			c.h = randv;
			c.l = randv;
			m[c.hl] = randv;

			c.pc = *rc::gen::suchThat(rc::gen::inRange(0, 10),
					[&c](int x){ 
						return x != c.hl && (x+1) != c.hl;
					});

			m[c.pc] = 0xcb;
			const u8 cb = *rc::gen::inRange(0x0, 0x7);
			m[c.pc + 1] = cb;

			cpu::step(c, m);

			RC_ASSERT(c.cf == !!(randv & (1 << 7)));
			RC_ASSERT(c.nf == false);
			RC_ASSERT(c.hf == false);

			switch (cb & 0x7) {
			case 0:
				RC_ASSERT(c.b == static_cast<u8>((randv << 1) | (randv >> 7)));
				RC_ASSERT(c.zf == (c.b == 0));
				break;
			case 1:
				RC_ASSERT(c.c == static_cast<u8>((randv << 1) | (randv >> 7)));
				RC_ASSERT(c.zf == (c.c == 0));
				break;
			case 2:
				RC_ASSERT(c.d == static_cast<u8>((randv << 1) | (randv >> 7)));
				RC_ASSERT(c.zf == (c.d == 0));
				break;
			case 3:
				RC_ASSERT(c.e == static_cast<u8>((randv << 1) | (randv >> 7)));
				RC_ASSERT(c.zf == (c.e == 0));
				break;
			case 4:
				RC_ASSERT(c.h == static_cast<u8>((randv << 1) | (randv >> 7)));
				RC_ASSERT(c.zf == (c.h == 0));
				break;
			case 5:
				RC_ASSERT(c.l == static_cast<u8>((randv << 1) | (randv >> 7)));
				RC_ASSERT(c.zf == (c.l == 0));
				break;
			case 6:
				RC_ASSERT(m[c.hl] == static_cast<u8>((randv << 1) | (randv >> 7)));
				RC_ASSERT(c.zf == (m[c.hl] == 0));
				break;
			case 7:
				RC_ASSERT(c.a == static_cast<u8>((randv << 1) | (randv >> 7)));
				RC_ASSERT(c.zf == (c.a == 0));
				break;
			}
		});
	}
}
