include $(BEGIN_MODULE)

MODULE:=utl

INC+=public

SRC+=word.cpp
TST_SRC+=test/word_test.cpp
TST_SRC+=test/types_test.cpp

include $(END_MODULE)
