#pragma once

#include "types.h"
#include "word.h"

namespace cpu {

// `cpu` comprises the state (program counter, stack pointer and registers) of
// the Sharp LR35902.
struct cpu {
	u8 a{0U};
	u8 f{0U};
	u8 b{0U};
	u8 c{0U};
	u8 d{0U};
	u8 e{0U};
	u8 h{0U};
	u8 l{0U};

	word af{a, f};
	word bc{b, c};
	word de{d, e};
	word hl{h, l};

	u16 pc{0U};
	u16 sp{0xffffU};
};

void step(cpu&, mmu_ref) noexcept;

}