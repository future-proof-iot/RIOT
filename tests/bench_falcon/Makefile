include ../Makefile.tests_common

USEMODULE += xtimer
USEMODULE += random
USEPKG += falcon_sig

CFLAGS += -DFALCON_SIGS_STACKSIZE=50000
CFLAGS += -DTHREAD_STACKSIZE_MAIN=20000

include $(RIOTBASE)/Makefile.include
