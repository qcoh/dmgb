include $(BEGIN_TEST_MODULE)

MODULE:=tst

TST_SRC+=test.cpp
TST_INC+=catch2/single_include/catch2

include $(END_TEST_MODULE)
