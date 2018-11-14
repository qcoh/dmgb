#include "test.h"

#include "cpu.h"

u8& reg(cpu::cpu& c, u8* m, const u8 index) {
	switch (index & 0x7) {
	case 0: return c.b;
	case 1: return c.c;
	case 2: return c.d;
	case 3: return c.e;
	case 4: return c.h;
	case 5: return c.l;
	case 6: return m[c.hl];
	case 7: return c.a;
	}
	// impossible
	return c.a;
}

u16 reg16(const cpu::cpu& c, const u8 index) {
	if (index == 0) {
		return c.bc;
	} else if (index == 1) {
		return c.de;
	} else if (index == 2) {
		return c.hl;
	} else {
		return c.sp;
	}
}	

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

			const u8 Mask = 1 << ((cb >> 3) & 0x7);

			RC_ASSERT((reg(c, m, cb & 0x7) & Mask) == 0);
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

			const u8 Mask = 1 << ((cb >> 3) & 0x7);

			RC_ASSERT((reg(c, m, cb & 0x7) & Mask) != 0);
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

			RC_ASSERT(reg(c, m, cb & 0x7) == static_cast<u8>((randv << 1) | (randv >> 7)));
			RC_ASSERT(c.zf == (reg(c, m, cb & 0x7) == 0));
		});
	}
}


SCENARIO("rrc", "[cpu]") {
	GIVEN("cpu and mmu") {
		cpu::cpu c{};
		u8 m[0x10000] = {0};

		rc::PROPERTY("rrc",
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
			const u8 cb = *rc::gen::inRange(0x8, 0xf);
			m[c.pc + 1] = cb;

			cpu::step(c, m);

			RC_ASSERT(c.cf == !!(randv & 1));
			RC_ASSERT(c.nf == false);
			RC_ASSERT(c.hf == false);

			RC_ASSERT(reg(c, m, cb & 0x7) == static_cast<u8>((randv >> 1) | (randv << 7)));
			RC_ASSERT(c.zf == (reg(c, m, cb & 0x7) == 0));
		});
	}
}


SCENARIO("rl", "[cpu]") {
	GIVEN("cpu and mmu") {
		cpu::cpu c{};
		u8 m[0x10000] = {0};

		rc::PROPERTY("rl",
		[&c, &m](const u8 randv, const bool randcf) {
			c.a = randv;
			c.b = randv;
			c.c = randv;
			c.d = randv;
			c.e = randv;
			c.h = randv;
			c.l = randv;
			m[c.hl] = randv;
			c.cf = randcf;

			c.pc = *rc::gen::suchThat(rc::gen::inRange(0, 10),
					[&c](int x){ 
						return x != c.hl && (x+1) != c.hl;
					});

			m[c.pc] = 0xcb;
			const u8 cb = *rc::gen::inRange(0x10, 0x17);
			m[c.pc + 1] = cb;

			cpu::step(c, m);

			RC_ASSERT(c.cf == !!(randv & (1 << 7)));
			RC_ASSERT(c.nf == false);
			RC_ASSERT(c.hf == false);

			RC_ASSERT(reg(c, m, cb & 0x7) == static_cast<u8>((randv << 1) | randcf));
			RC_ASSERT(c.zf == (reg(c, m, cb & 0x7) == 0));
		});
	}
}

SCENARIO("rr", "[cpu]") {
	GIVEN("cpu and mmu") {
		cpu::cpu c{};
		u8 m[0x10000] = {0};

		rc::PROPERTY("rr",
		[&c, &m](const u8 randv, const bool randcf) {
			c.a = randv;
			c.b = randv;
			c.c = randv;
			c.d = randv;
			c.e = randv;
			c.h = randv;
			c.l = randv;
			m[c.hl] = randv;
			c.cf = randcf;

			c.pc = *rc::gen::suchThat(rc::gen::inRange(0, 10),
					[&c](int x){ 
						return x != c.hl && (x+1) != c.hl;
					});

			m[c.pc] = 0xcb;
			const u8 cb = *rc::gen::inRange(0x18, 0x1f);
			m[c.pc + 1] = cb;

			cpu::step(c, m);

			RC_ASSERT(c.cf == !!(randv & 1));
			RC_ASSERT(c.nf == false);
			RC_ASSERT(c.hf == false);

			RC_ASSERT(reg(c, m, cb & 0x7) == static_cast<u8>((randv >> 1) | (randcf << 7)));
			RC_ASSERT(c.zf == (reg(c, m, cb & 0x7) == 0));
		});
	}
}

SCENARIO("sla", "[cpu]") {
	GIVEN("cpu and mmu") {
		cpu::cpu c{};
		u8 m[0x10000] = {0};

		rc::PROPERTY("sla",
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
			const u8 cb = *rc::gen::inRange(0x20, 0x27);
			m[c.pc + 1] = cb;

			cpu::step(c, m);

			RC_ASSERT(c.cf == !!(randv & (1 << 7)));
			RC_ASSERT(c.nf == false);
			RC_ASSERT(c.hf == false);

			RC_ASSERT(reg(c, m, cb & 0x7) == static_cast<u8>(randv << 1));
			RC_ASSERT(c.zf == (reg(c, m, cb & 0x7) == 0));
		});
	}
}

SCENARIO("sra", "[cpu]") {
	GIVEN("cpu and mmu") {
		cpu::cpu c{};
		u8 m[0x10000] = {0};

		rc::PROPERTY("sra",
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
			const u8 cb = *rc::gen::inRange(0x28, 0x2f);
			m[c.pc + 1] = cb;

			cpu::step(c, m);

			RC_ASSERT(c.cf == !!(randv & 1));
			RC_ASSERT(c.nf == false);
			RC_ASSERT(c.hf == false);

			RC_ASSERT(reg(c, m, cb & 0x7) == static_cast<u8>(randv >> 1) | (randv & (1 << 7)));
			RC_ASSERT(c.zf == (reg(c, m, cb & 0x7) == 0));
		});
	}
}

SCENARIO("swap", "[cpu]") {
	GIVEN("cpu and mmu") {
		cpu::cpu c{};
		u8 m[0x10000] = {0};

		rc::PROPERTY("swap",
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
			const u8 cb = *rc::gen::inRange(0x30, 0x37);
			m[c.pc + 1] = cb;

			cpu::step(c, m);

			RC_ASSERT(c.cf == false);
			RC_ASSERT(c.nf == false);
			RC_ASSERT(c.hf == false);

			RC_ASSERT(reg(c, m, cb & 0x7) == static_cast<u8>((randv >> 4) | (randv << 4)));
			RC_ASSERT(c.zf == (reg(c, m, cb & 0x7) == 0));
		});
	}
}

SCENARIO("srl", "[cpu]") {
	GIVEN("cpu and mmu") {
		cpu::cpu c{};
		u8 m[0x10000] = {0};

		rc::PROPERTY("srl",
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
			const u8 cb = *rc::gen::inRange(0x38, 0x3f);
			m[c.pc + 1] = cb;

			cpu::step(c, m);

			RC_ASSERT(c.cf == !!(randv & 1));
			RC_ASSERT(c.nf == false);
			RC_ASSERT(c.hf == false);

			RC_ASSERT(reg(c, m, cb & 0x7) == static_cast<u8>(randv >> 1));
			RC_ASSERT(c.zf == (reg(c, m, cb & 0x7) == 0));
		});
	}
}

SCENARIO("ld_16", "[cpu]") {
	GIVEN("cpu and mmu") {
		cpu::cpu c{};
		u8 m[0x10000] = {0};

		rc::PROPERTY("ld_16",
		[&c, &m](const u16 randw) {
			m[c.pc] = *rc::gen::element(0x1, 0x11, 0x21, 0x31);

			m[c.pc+1] = randw & 0xff;
			m[c.pc+2] = randw >> 8;

			cpu::step(c, m);

			RC_ASSERT(reg16(c, (m[c.pc] >> 4) & 0x3) == randw);
		});
	}
}

SCENARIO("inc", "[cpu]") {
	GIVEN("cpu and mmu") {
		cpu::cpu c{};
		u8 m[0x10000] = {0};

		rc::PROPERTY("inc",
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
			m[c.pc] = *rc::gen::element(0x4, 0xc, 0x14, 0x1c, 0x24, 0x2c, 0x34, 0x3c);

			cpu::step(c, m);

			RC_ASSERT(reg(c, m, (m[c.pc] >> 3) & 0x7) == static_cast<u8>(randv+1));
			RC_ASSERT(c.zf == (reg(c, m, (m[c.pc] >> 3) & 0x7) == 0));
			RC_ASSERT(c.nf == false);
			RC_ASSERT(c.hf == ((randv & 0xf) == 0xf));
		});
	}
}

SCENARIO("dec", "[cpu]") {
	GIVEN("cpu and mmu") {
		cpu::cpu c{};
		u8 m[0x10000] = {0};

		rc::PROPERTY("dec",
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
			m[c.pc] = *rc::gen::element(0x5, 0xd, 0x15, 0x1d, 0x25, 0x2d, 0x35, 0x3d);

			cpu::step(c, m);

			RC_ASSERT(reg(c, m, (m[c.pc] >> 3) & 0x7) == static_cast<u8>(randv-1));
			RC_ASSERT(c.zf == (reg(c, m, (m[c.pc] >> 3) & 0x7) == 0));
			RC_ASSERT(c.nf == true);
			RC_ASSERT(c.hf == ((randv & 0xf) == 0));
		});
	}
}

SCENARIO("inc16", "[cpu]") {
	GIVEN("cpu and mmu") {
		cpu::cpu c{};
		u8 m[0x10000] = {0};

		rc::PROPERTY("inc16",
		[&c, &m](const u16 randw) {
			c.bc = randw;
			c.de = randw;
			c.hl = randw;
			c.sp = randw;

			m[c.pc] = *rc::gen::element(0x3, 0x13, 0x23, 0x33);

			cpu::step(c, m);

			RC_ASSERT(reg16(c, (m[c.pc] >> 4) & 0x3) == static_cast<u16>(randw+1));
		});
	}
}

SCENARIO("dec16", "[cpu]") {
	GIVEN("cpu and mmu") {
		cpu::cpu c{};
		u8 m[0x10000] = {0};

		rc::PROPERTY("dec16",
		[&c, &m](const u16 randw) {
			c.bc = randw;
			c.de = randw;
			c.hl = randw;
			c.sp = randw;

			m[c.pc] = *rc::gen::element(0xb, 0x1b, 0x2b, 0x3b);

			cpu::step(c, m);

			RC_ASSERT(reg16(c, (m[c.pc] >> 4) & 0x3) == static_cast<u16>(randw-1));
		});
	}
}
