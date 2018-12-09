#pragma once

#include "types.h"

namespace cpu {

// The mmu, as far as the cpu interpreter cares.
class read_writer {
public:
	virtual u8 read(const u16) const noexcept;
	virtual void write(const u16, const u8) noexcept;

};

}
