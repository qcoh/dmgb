TMP_TST_SRC+=$(addprefix $(MODULE)/, $(TST_SRC))
TMP_TST_INC+=$(addprefix $(MODULE)/, $(TST_INC))

TST_OBJ+=$(addprefix $(MODULE)/,$(patsubst %.cpp,%.o,$(TST_SRC)))

TST_SRC:=$(TMP_TST_SRC)
TST_INC:=$(TMP_TST_INC)
