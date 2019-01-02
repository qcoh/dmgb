#pragma once

#include "types.h"

namespace cpu
{

struct mmu
{
	virtual u8 read(const u16) const noexcept = 0;
	virtual void write(const u16, const u8) noexcept = 0;
};

} // namespace cpu