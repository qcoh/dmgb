#pragma once

#include <iosfwd>

#include <catch2/catch.hpp>
#include "rapidcheck.h"

// rapidcheck
// https://github.com/emil-e/rapidcheck/blob/master/extras/catch/include/rapidcheck/catch.h

namespace rc {
template <typename Testable>
void PROPERTY(const std::string& description, Testable&& testable) {
  using namespace detail;

#ifdef CATCH_CONFIG_PREFIX_ALL
  CATCH_SECTION(description) {
#else
  SECTION(description) {
#endif

    const auto result = checkTestable(std::forward<Testable>(testable));

    if (result.template is<SuccessResult>()) {
      const auto success = result.template get<SuccessResult>();
      if (!success.distribution.empty()) {
        std::cout << "- " << description << std::endl;
        printResultMessage(result, std::cout);
        std::cout << std::endl;
      }
    } else {
      std::ostringstream ss;
      printResultMessage(result, ss);
#ifdef CATCH_CONFIG_PREFIX_ALL
      CATCH_INFO(ss.str() << "\n");
      CATCH_FAIL();
#else
      INFO(ss.str() << "\n");
      FAIL();
#endif
    }
  }
}
}  // namespace rc
