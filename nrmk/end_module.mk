TMP_SRC+=$(addprefix $(MODULE)/, $(SRC))
TMP_INC+=$(addprefix $(MODULE)/, $(INC))
TMP_TST_SRC+=$(addprefix $(MODULE)/, $(TST_SRC))
TMP_TST_INC+=$(addprefix $(MODULE)/, $(TST_INC))

OBJ+=$(addprefix $(MODULE)/,$(patsubst %.cpp,%.o,$(SRC)))
TST_OBJ+=$(addprefix $(MODULE)/,$(patsubst %.cpp,%.o,$(TST_SRC)))

SRC:=$(TMP_SRC)
INC:=$(TMP_INC)
TST_SRC:=$(TMP_TST_SRC)
TST_INC:=$(TMP_TST_INC)
