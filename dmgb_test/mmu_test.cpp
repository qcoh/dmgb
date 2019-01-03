#include "test.h"

#include "bios.h"
#include "mmu.h"
#include "read_writer.h"

struct test_mapper : public read_writer {
  u8 m_data[0x10000] = {0};

  u8 read(const u16 addr) const noexcept override { return m_data[addr]; }
  void write(const u16 addr, const u8 v) noexcept override { m_data[addr] = v; }
};

SCENARIO("writing to mapper", "[mmu]") {
  GIVEN("an mmu object") {
    read_writer* bios = nullptr;
    test_mapper mapper{};
    mmu::mmu m{*bios, mapper};

    WHEN("writing to mapper") {
      m.write(0x101, 123);

      THEN("value at 0x101 is 123") { REQUIRE(m.read(0x101) == 123); }
    }
  }
}
