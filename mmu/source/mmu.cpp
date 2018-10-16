#include "mmu.h"
#include "bios.h"

#include <gsl/gsl_util>

#include <stdexcept>
#include <system_error>

#include <unistd.h>
#include <sys/mman.h>

namespace {
	const size_t MemorySize = 0x10000U;
}

namespace mmu {

mmu* mmu::s_mmu{nullptr};

struct mmu::ref_wrapper {
	u8 arr[MemorySize];
};

mmu::mmu(const bios& bios)
: m_mmu{nullptr}
, m_ref_wrapper{nullptr}
, m_bios{bios}
{
	if (s_mmu != nullptr) {
		throw std::runtime_error{"there must not exist more than one mmu"};
	}

	if (sysconf(_SC_PAGE_SIZE) != 4096) {
		throw std::runtime_error{"unexpected page size"};
	}

	bool cleanup = true;

	m_mmu = static_cast<u8*>(mmap(nullptr,
				MemorySize,
				PROT_READ,
				MAP_PRIVATE|MAP_ANONYMOUS,
				-1,
				0));
	if (m_mmu == MAP_FAILED) {
		throw std::system_error{errno, std::generic_category()};
	}
	auto _ = gsl::finally([&]{if (cleanup) munmap(m_mmu, MemorySize); });

	cleanup = false;

	m_ref_wrapper = new (m_mmu) ref_wrapper;
}

mmu::~mmu() {
	s_mmu = nullptr;

	munmap(m_mmu, MemorySize);
}

mmu::operator mmu_ref () const noexcept {
	return m_ref_wrapper->arr;
}

}
