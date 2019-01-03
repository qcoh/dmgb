#include "bios.h"

#include <istream>
#include <iterator>
#include <stdexcept>

#include <assert.h>

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

u8 bios::read(const u16 addr) const noexcept {
  assert(addr < 0x100);
  return m_data[addr];
}

void bios::write(const u16 addr, const u8 v) noexcept {
  assert(addr < 0x100);
  m_data[addr] = v;
}

}  // namespace mmu
