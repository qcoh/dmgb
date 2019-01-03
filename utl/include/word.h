#pragma once

#include "types.h"

class word {
 public:
  word(u8&, u8&) noexcept;
  word& operator=(const u16) noexcept;
  operator u16() const noexcept;

 private:
  u8& m_hi;
  u8& m_lo;
};
