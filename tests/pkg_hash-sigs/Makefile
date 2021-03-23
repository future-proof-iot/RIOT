include ../Makefile.tests_common

MAIN_THREAD_SIZE = THREAD_STACKSIZE_DEFAULT+17000
CFLAGS += -DTHREAD_STACKSIZE_MAIN=\($(MAIN_THREAD_SIZE)\)

USEMODULE += embunit
USEMODULE += random
USEPKG += hash-sigs

include $(RIOTBASE)/Makefile.include
