BIN:=dmgb
TST:=test
MODULES:=cpu mmu utl tst main
BUILD_SYSTEM=nrmk

CXXFLAGS:=-Wall -Wextra -pedantic -std=c++17

include $(BUILD_SYSTEM)/buildsystem.mk
