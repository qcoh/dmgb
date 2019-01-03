#include <exception>
#include <fstream>
#include <iostream>

#include "bios.h"
#include "cpu.h"
#include "mmu.h"
#include "rom_only.h"

int main() {
  try {
    std::ifstream bios_data{"bios.bin"};
    mmu::bios bios{bios_data};

    std::ifstream rom_data{"rom.bin"};
    mmu::rom_only rom{rom_data};

    mmu::mmu mmu{bios, rom};
    cpu::cpu cpu{};

    for (;;) {
      cpu::step(cpu, mmu);
    }
  } catch (std::exception& e) {
    std::cerr << e.what() << '\n';
  }
}
