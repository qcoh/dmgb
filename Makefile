EXECUTABLE:=dmgb
TEST_EXECUTABLE:=test
MODULES:=cpu mmu
BUILD_SYSTEM=nrmk

CXXFLAGS:=-Wall -Wextra -pedantic -std=c++17

include $(BUILD_SYSTEM)/buildsystem.mk
