#include "test.h"

#include "mmu.h"
#include "bios.h"

SCENARIO("writing to r/o page", "[mmu]")
{
	GIVEN("an mmu object")
	{
		mmu::bios *b = nullptr;
		mmu::mmu m{*b};

		WHEN("writing 123 to location 0")
		{
			m[0] = 123;

			THEN("value at 0 is not 123")
			{
				REQUIRE(m[0] != 123);
			}
		}
	}
}
