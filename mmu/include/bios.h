#pragma once

#include <iosfwd>

#include "read_writer.h"
#include "types.h"

namespace mmu {

// bios provides mmu with the bios, the "initialization program" run at startup.
class bios : public read_writer {
 public:
  explicit bios(std::istream&);

  bios(const bios&) = delete;
  bios& operator=(const bios&) = delete;
  bios(bios&&) = delete;
  bios& operator=(bios&&) = delete;

  u8 read(const u16) const noexcept override;
  void write(const u16, const u8) noexcept override;

 private:
  u8 m_data[0x100] = {0U};
};

}  // namespace mmu
