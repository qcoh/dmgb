#pragma once

#include "types.h"
#include "read_writer.h"

namespace cpu {

template <typename T>
class rw_ref;

template <>
class rw_ref<mmu_ref> {
public:
	rw_ref(mmu_ref mmu, const u16 addr) noexcept
	: m_mmu{mmu}
	, m_addr{addr}
	{
	}

	void operator=(const u8 v) noexcept {
		m_mmu[m_addr] = v;
	}

	operator u8 () const noexcept {
		return m_mmu[m_addr];
	}

private:
	mmu_ref& m_mmu;
	const u16 m_addr;
};

template <>
class rw_ref<read_writer> {
public:
	rw_ref(read_writer& mmu, const u16 addr) noexcept
	: m_mmu{mmu}
	, m_addr{addr}
	{
	}

	void operator=(const u8 v) noexcept {
		m_mmu.write(m_addr, v);
	}

	operator u8 () const noexcept {
		return m_mmu.read(m_addr);
	}

private:
	read_writer& m_mmu;
	const u16 m_addr;
};

}
