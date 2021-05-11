include ../Makefile.tests_common

USEMODULE += embunit
USEMODULE += random
USEPKG += lattice-sigs

USEMODULE += fmt

#BLOBS += sk.txt mat.txt pk.txt
#DIRS += blob_module
#USEMODULE += blob_module
CFLAGS += -DLATTICE_SIGS_STACKSIZE=54138
CFLAGS += -DTHREAD_STACKSIZE_MAIN=105000

include $(RIOTBASE)/Makefile.include
