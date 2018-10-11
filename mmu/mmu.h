#pragma once

#include "types.h"

namespace mmu {

// mmu wires bios, cartridge, ram and IO registers together.
class mmu {
public:
	mmu();
	operator mmu_ref () const noexcept;

private:
	u8* m_mmu;

	struct ref_wrapper;
	ref_wrapper* m_ref_wrapper;
};

}
