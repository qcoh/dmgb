#pragma once

#include "read_writer.h"
#include "types.h"

namespace mmu {

class bios;

// mmu wires bios, cartridge, ram and IO registers together.
class mmu : public read_writer {
 public:
  mmu(const read_writer&, read_writer&);

  mmu(const mmu&) = delete;
  mmu& operator=(const mmu&) = delete;
  mmu(mmu&&) = delete;
  mmu& operator=(mmu&&) = delete;

  u8 read(const u16) const noexcept override;
  void write(const u16, const u8) noexcept override;

 private:
  bool m_bios_mode = true;
  const read_writer& m_bios;
  read_writer& m_mapper;

  u8 m_vram[0x2000] = {0};
  u8 m_wram[0x1000] = {0};
};

}  // namespace mmu
