#include "bios.h"

#include <istream>
#include <stdexcept>
#include <iterator>

namespace mmu {

bios::bios(std::istream& f) {
	auto it = std::istreambuf_iterator<char>{f};
	auto end = std::istreambuf_iterator<char>{};
	for (int i = 0; i < 0x100; i++) {
		if (it == end) {
			// fewer than 0x100 bytes in file
			throw std::runtime_error{"bios file corrupted"};
		}

		m_data[i] = static_cast<u8>(*it);
		++it;
	}

	if (it != end) {
		// more than 0x100 bytes in file
		throw std::runtime_error{"bios file corrupted"};
	}
}

bios::operator bios_ref () const noexcept {
	return m_data;
}

}
