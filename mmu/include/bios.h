#pragma once

#include <iosfwd>

#include "types.h"

namespace mmu {

// bios provides mmu with the bios, the "initialization program" run at startup.
class bios {
	using bios_ref = const u8 (&) [0x100];
public:
	explicit bios(std::istream&);

	operator bios_ref () const noexcept;

private:
	u8 m_data[0x100] = {0U};
};

}
