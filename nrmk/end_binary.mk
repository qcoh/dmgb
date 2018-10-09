TMP_BIN_SRC+=$(addprefix $(MODULE)/, $(BIN_SRC))
BIN_OBJ+=$(addprefix $(MODULE)/,$(patsubst %.cpp,%.o,$(BIN_SRC)))
BIN_SRC:=$(TMP_BIN_SRC)
