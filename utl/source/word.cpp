#include "word.h"

word::word(u8& hi, u8& lo) noexcept : m_hi{hi}, m_lo{lo} {}

word& word::operator=(const u16 rhs) noexcept {
  m_hi = rhs >> 8;
  m_lo = static_cast<u8>(rhs);

  return *this;
}

word::operator u16() const noexcept {
  return (m_hi << 8) + m_lo;
}
