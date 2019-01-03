#pragma once

#include "cpu_mmu.h"
#include "types.h"

namespace cpu {

class rw_ref {
 public:
  inline rw_ref(mmu& mmu, const u16 addr) noexcept : m_mmu{mmu}, m_addr{addr} {}

  inline void operator=(const u8 v) noexcept { m_mmu.write(m_addr, v); }

  inline operator u8() const noexcept { return m_mmu.read(m_addr); }

 private:
  mmu& m_mmu;
  const u16 m_addr;
};

}  // namespace cpu
