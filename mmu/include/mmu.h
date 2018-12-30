#pragma once

#include "types.h"

namespace mmu {

class bios;

// mmu wires bios, cartridge, ram and IO registers together.
class mmu {
public:
	explicit mmu(const bios&);
	~mmu();

	mmu(const mmu&) = delete;
	mmu& operator=(const mmu&) = delete;
	mmu(mmu&&) = delete;
	mmu& operator=(mmu&&) = delete;

	operator mmu_ref () const noexcept;

private:
	u8* m_mmu;

	struct ref_wrapper;
	ref_wrapper* m_ref_wrapper;

	const bios& m_bios;
};

}
