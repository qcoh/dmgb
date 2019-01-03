#pragma once

#include "types.h"

struct read_writer {
  virtual u8 read(const u16) const noexcept = 0;
  virtual void write(const u16, const u8) noexcept = 0;
};