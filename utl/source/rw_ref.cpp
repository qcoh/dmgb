#include "rw_ref.h"
#include "read_writer.h"

rw_ref::rw_ref(read_writer& rw, const u16 addr) noexcept
    : m_rw{rw}, m_addr{addr} {}

void rw_ref::operator=(const u8 v) noexcept {
  m_rw.write(m_addr, v);
}

rw_ref::operator u8() const noexcept {
  return m_rw.read(m_addr);
}