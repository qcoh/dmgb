LIB_OBJ:=$(addprefix $(MODULE)/, $(patsubst %.cpp,%.o,$(SRC)))
TST_LIB_OBJ:=$(addprefix $(MODULE)/, $(patsubst %.cpp,%.o,$(TST_SRC)))

$(MODULE)/lib$(MODULE).a: $(LIB_OBJ)
	ar rcs $@ $^

LFLAGS+=-L$(MODULE) -l$(MODULE)
LIB+=$(MODULE)/lib$(MODULE).a

TMP_SRC+=$(addprefix $(MODULE)/, $(SRC))
TMP_OBJ+=$(LIB_OBJ)
TMP_INC+=$(addprefix $(MODULE)/, $(INC))
TMP_TST_SRC+=$(addprefix $(MODULE)/, $(TST_SRC))
TMP_TST_OBJ+=$(TST_LIB_OBJ)

SRC:=$(TMP_SRC)
OBJ:=$(TMP_OBJ)
INC:=$(TMP_INC)
TST_SRC:=$(TMP_TST_SRC)
TST_OBJ:=$(TMP_TST_OBJ)
