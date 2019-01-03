#pragma once

#include "types.h"

class read_writer;

class rw_ref {
 public:
  rw_ref(read_writer&, const u16) noexcept;
  void operator=(const u8 v) noexcept;
  operator u8() const noexcept;

 private:
  read_writer& m_rw;
  const u16 m_addr;
};
