#pragma once

#include "types.h"

namespace cpu {

template <int I>
class bitref {
private:
	static_assert(0 <= I && I < 8);
	static constexpr u8 Mask = static_cast<u8>(1 << I);
	static constexpr u8 InvMask = static_cast<u8>(~Mask);

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
