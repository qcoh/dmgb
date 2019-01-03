#include "word.h"
#include "test.h"

SCENARIO("Endianess of word", "[utl]") {
  GIVEN("A word comprising hi and lo bytes") {
    u8 hi{0U};
    u8 lo{0U};

    word w{hi, lo};

    rc::PROPERTY("modifying word changes hi/lo bytes",
                 [&hi, &lo, &w](const u16 randw) {
                   w = randw;

                   RC_ASSERT(hi == (randw >> 8));
                   RC_ASSERT(lo == (randw & 0xff));
                 });

    rc::PROPERTY("modifying hi/lo bytes changes word",
                 [&hi, &lo, &w](const u8 randhi, const u8 randlo) {
                   hi = randhi;
                   lo = randlo;

                   const u16 randw = randhi << 8 | randlo;
                   RC_ASSERT(w == randw);
                 });
  }
}
