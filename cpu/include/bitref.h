#pragma once

#include "types.h"

namespace cpu {

template <int I, u8 Mask = (1 << I), u8 InvMask = static_cast<u8>(~Mask)>
class bitref {
static_assert(0 <= I && I < 8);
public:
	bitref(u8& ref) noexcept : m_ref{ref} {}
	~bitref() = default;

	bitref(const bitref&) = delete;
	bitref& operator=(const bitref&) = delete;
	bitref(bitref&&) = delete;
	bitref& operator=(bitref&&) = delete;

	operator bool () const noexcept {
		return m_ref & Mask;
	}

	bitref& operator=(const bool rhs) noexcept {
		m_ref = (m_ref & InvMask) | (rhs << I);
		return *this;
	}

private:
	u8& m_ref;
};

}
