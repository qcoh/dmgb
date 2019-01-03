#define _GNU_SOURCE 1 /* rXX registers in signal handler */

#include "mmu.h"
#include "bios.h"

#include <gsl/gsl_util>

#include <stdexcept>
#include <system_error>

#include <assert.h>
#include <signal.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

namespace {
const size_t MemorySize = 0x10000U;

mmu::mmu* s_mmu = nullptr;

void sigsegv_handler(int /*signum*/, siginfo_t* /*si*/, void* mcontext) {
  assert(s_mmu != nullptr);

  ucontext_t* context = (ucontext_t*)mcontext;
  const bool isWrite = context->uc_mcontext.gregs[REG_ERR] & 0x2;
  (void)isWrite;

  context->uc_mcontext.gregs[REG_RIP] += 3;
}
}  // namespace

namespace mmu {

struct mmu::ref_wrapper {
  u8 arr[MemorySize];
};

mmu::mmu(const bios& bios)
    : m_mmu{nullptr}, m_ref_wrapper{nullptr}, m_bios{bios} {
  // make sure no other instance of mmu exists
  if (s_mmu != nullptr) {
    throw std::runtime_error{"there must not exist more than one mmu"};
  }

  // make sure that the page size is 4kb
  if (sysconf(_SC_PAGE_SIZE) != 4096) {
    throw std::runtime_error{"unexpected page size"};
  }

  bool cleanup = true;

  // allocate memory map
  m_mmu = static_cast<u8*>(
      mmap(nullptr, MemorySize, PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
  if (m_mmu == MAP_FAILED) {
    throw std::system_error{errno, std::generic_category()};
  }
  auto _ = gsl::finally([&] {
    if (cleanup)
      munmap(m_mmu, MemorySize);
  });

  // register signal handler
  struct sigaction action;
  memset(&action, 0, sizeof(action));
  action.sa_flags = SA_SIGINFO;
  action.sa_sigaction = sigsegv_handler;
  if (sigaction(SIGSEGV, &action, NULL) == -1) {
    throw std::system_error{errno, std::generic_category()};
  }

  m_ref_wrapper = new (m_mmu) ref_wrapper;
  s_mmu = this;
  cleanup = false;
}

mmu::~mmu() {
  s_mmu = nullptr;

  munmap(m_mmu, MemorySize);
}

mmu::operator mmu_ref() const noexcept {
  return m_ref_wrapper->arr;
}

}  // namespace mmu
