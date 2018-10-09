.DEFAULT_GOAL:=all

BEGIN_MODULE:=$(BUILD_SYSTEM)/begin_module.mk
END_MODULE:=$(BUILD_SYSTEM)/end_module.mk

BEGIN_TEST_MODULE:=$(BUILD_SYSTEM)/begin_test_module.mk
END_TEST_MODULE:=$(BUILD_SYSTEM)/end_test_module.mk

SRC:=
TST_SRC:=

INC:=
TST_INC:=

OBJ:=
TST_OBJ:=

include $(abspath $(patsubst %,%/module.mk,$(MODULES)))

CXXFLAGS+=$(patsubst %,-I%,$(INC))

TST_CXXFLAGS:=$(patsubst %,-I%,$(TST_INC))

all: $(BIN) $(TST)

$(BIN): $(BIN).cpp $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ): %.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TST): $(OBJ) $(TST_OBJ)
	$(CXX) $^ -o $@

$(TST_OBJ): %.o : %.cpp
	$(CXX) $(CXXFLAGS) $(TST_CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f $(BIN) $(TST) $(OBJ) $(TST_OBJ) 
