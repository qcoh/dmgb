#include "mmu.h"

namespace mmu {

struct mmu::ref_wrapper {
	u8 arr[0x10000];
};

mmu::mmu()
: m_mmu{nullptr}
, m_ref_wrapper{nullptr}
{
}

mmu::operator mmu_ref () const noexcept {
	return m_ref_wrapper->arr;
}

}
