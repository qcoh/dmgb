BEGIN_MODULE=$(BUILD_SYSTEM)/begin_module.mk
END_MODULE=$(BUILD_SYSTEM)/end_module.mk

include $(abspath $(patsubst %,%/module.mk,$(MODULES)))

OBJ:=$(patsubst %.cpp,%.o,$(SRC))
TST_OBJ:=$(patsubst %.cpp,%.o,$(TST_SRC))

$(EXECUTABLE): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $@

$(OBJ): %.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_EXECUTABLE): $(OBJ) $(TST_OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) $(TST_OBJ) -o $@

$(TST_OBJ): %.o : %.cpp
	$(CXX) $(CXXFLAGS) -c -g $< -o $@
