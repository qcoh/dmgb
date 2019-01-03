#include "rom_only.h"

#include <fstream>

namespace mmu {
rom_only::rom_only(std::ifstream& f) {
  auto it = std::istreambuf_iterator<char>{f};
  auto end = std::istreambuf_iterator<char>{};
  for (int i = 0; i < 0x8000; i++) {
    if (it == end) {
      // fewer than 0x8000 bytes in file
      throw std::runtime_error{"rom file corrupted"};
    }

    m_rom[i] = static_cast<u8>(*it);
    ++it;
  }

  if (it != end) {
    // more than 0x8000 bytes in file
    throw std::runtime_error{"rom file corrupted"};
  }
}

u8 rom_only::read(const u16 addr) const noexcept {
  if (addr < 0x8000) {
    return m_rom[addr];
  } else if (0xa000 <= addr && addr < 0xc000) {
    return m_ram[addr - 0xa000];
  }
  // TODO: handle error
  return 0;
}

void rom_only::write(const u16 addr, const u8 v) noexcept {
  if (addr < 0x8000) {
    m_rom[addr] = v;
  } else if (0xa000 <= addr && addr < 0xc000) {
    m_ram[addr - 0xa000] = v;
  }
  // TODO: handle error
}
}  // namespace mmu