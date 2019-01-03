
#include "mmu.h"
#include "bios.h"

// namespace

namespace mmu {

mmu::mmu(const read_writer& bios, read_writer& mapper)
    : m_bios{bios}, m_mapper{mapper} {}

u8 mmu::read(const u16 addr) const noexcept {
  switch (addr >> 12) {
    case 0x0:
    case 0x1:
    case 0x2:
    case 0x3:
      // bios and rom bank 0
      if (m_bios_mode && addr < 0x100) {
        return m_bios.read(addr);
      } else {
        return m_mapper.read(addr);
      }
    case 0x4:
    case 0x5:
    case 0x6:
    case 0x7:
      // rom bank 1 - ???
      return m_mapper.read(addr);
    case 0x8:
    case 0x9:
      // character ram, bg map data
      return m_vram[addr - 0x8000];
    case 0xa:
    case 0xb:
      // cartridge ram
      return m_mapper.read(addr);
    case 0xc:
      // internal ram bank 0
      return m_wram[addr - 0xc000];
    case 0xd:
      // internal ram bank 1 - 7 (CGB)
    case 0xe:
    case 0xf:
      // echo ram, object attribute memory, io registers, zero page, interrupt
      // enable flag
      break;
  }
  return 0;
}
void mmu::write(const u16 addr, const u8 v) noexcept {
  switch (addr >> 12) {
    case 0x0:
    case 0x1:
    case 0x2:
    case 0x3:
    case 0x4:
    case 0x5:
    case 0x6:
    case 0x7:
      // rom bank 0, 1 - ???
      m_mapper.write(addr, v);
      return;
    case 0x8:
    case 0x9:
      // character ram, bg map data
      m_vram[addr - 0x8000] = v;
      return;
    case 0xa:
    case 0xb:
      // cartridge ram
      m_mapper.write(addr, v);
      return;
    case 0xc:
      // internal ram bank 0
      m_wram[addr - 0xc000] = v;
      return;
    case 0xd:
    // internal ram bank 1 - 7 (CGB)
    case 0xe:
    case 0xf:
      // echo ram, object attribute memory, io registers, zero page,
      // interrupt enable flag
      break;
  }
}

}  // namespace mmu