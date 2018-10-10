.DEFAULT_GOAL:=all

BEGIN_MODULE:=$(BUILD_SYSTEM)/begin_module.mk
END_MODULE:=$(BUILD_SYSTEM)/end_module.mk

BEGIN_TEST_MODULE:=$(BUILD_SYSTEM)/begin_test_module.mk
END_TEST_MODULE:=$(BUILD_SYSTEM)/end_test_module.mk

BEGIN_BINARY:=$(BUILD_SYSTEM)/begin_binary.mk
END_BINARY:=$(BUILD_SYSTEM)/end_binary.mk

SRC:=
BIN_SRC:=
TST_SRC:=

INC:=
TST_INC:=

OBJ:=
BIN_OBJ:=
TST_OBJ:=

include $(abspath $(patsubst %,%/module.mk,$(MODULES)))

CXXFLAGS+=$(patsubst %,-I%,$(INC))
CXXFLAGS+=-MMD

TST_CXXFLAGS:=$(patsubst %,-I%,$(TST_INC))

all: $(BIN) $(TST)

$(BIN): $(OBJ) $(BIN_OBJ)
	$(CXX) $^ -o $@

$(OBJ): %.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_OBJ): %.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TST): $(OBJ) $(TST_OBJ)
	$(CXX) $^ -o $@

$(TST_OBJ): %.o : %.cpp
	$(CXX) $(CXXFLAGS) $(TST_CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f $(BIN) $(TST) $(OBJ) $(TST_OBJ) $(SRC:%.cpp=%.d) $(TST_SRC:%.cpp=%.d) $(BIN_SRC:%.cpp=%.d)

-include $(SRC:%.cpp=%.d)
-include $(TST_SRC:%.cpp=%.d)
-include $(BIN_SRC:%.cpp=%.d)
