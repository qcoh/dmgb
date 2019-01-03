#pragma once

#include <iosfwd>

#include "read_writer.h"
#include "types.h"

namespace mmu {

class rom_only : public read_writer {
 public:
  explicit rom_only(std::ifstream&);

  u8 read(const u16) const noexcept override;
  void write(const u16, const u8) noexcept override;

 private:
  u8 m_rom[0x8000] = {0};
  u8 m_ram[0x2000] = {0};
};

}  // namespace mmu