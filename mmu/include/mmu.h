#pragma once

#include "types.h"
#include "read_writer.h"

namespace mmu {

class bios;

// mmu wires bios, cartridge, ram and IO registers together.
class mmu : public read_writer {
 public:
  explicit mmu(const bios&);
  ~mmu();

  mmu(const mmu&) = delete;
  mmu& operator=(const mmu&) = delete;
  mmu(mmu&&) = delete;
  mmu& operator=(mmu&&) = delete;

  u8 read(const u16) const noexcept override;
  void write(const u16, const u8) noexcept override;

  operator mmu_ref() const noexcept;

 private:
  u8* m_mmu;

  struct ref_wrapper;
  ref_wrapper* m_ref_wrapper;

  const bios& m_bios;
};

}  // namespace mmu
