.DEFAULT_GOAL:=$(EXECUTABLE)

BEGIN_MODULE:=$(BUILD_SYSTEM)/begin_module.mk
END_MODULE:=$(BUILD_SYSTEM)/end_module.mk

LFLAGS:=
LIB:=

TST_LFLAGS:=
TST_LIB:=

INCLUDE:=

include $(abspath $(patsubst %,%/module.mk,$(MODULES)))

CXXFLAGS+=$(patsubst %,-I%,$(INCLUDE))

$(EXECUTABLE): $(EXECUTABLE).cpp $(LIB)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ): %.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_EXECUTABLE): $(TEST_EXECUTABLE).cpp $(TST_LIB) $(LIB)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TST_OBJ): %.o : %.cpp
	$(CXX) $(CXXFLAGS) -c -g $< -o $@

.PHONY: clean

clean:
	rm -f $(EXECUTABLE) $(TEST_EXECUTABLE) $(OBJ) $(TST_OBJ) $(LIB) $(TST_LIB)
