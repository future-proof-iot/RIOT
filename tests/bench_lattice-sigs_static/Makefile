include ../Makefile.tests_common

USEMODULE += xtimer
USEMODULE += random
USEPKG += lattice-sigs

CFLAGS += -DFLASH
CFLAGS += -DLATTICE_SIGS_STACKSIZE=54138
CFLAGS += -DTHREAD_STACKSIZE_MAIN=105000

include $(RIOTBASE)/Makefile.include
