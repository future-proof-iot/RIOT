DEVELHELP ?= 0
include ../Makefile.tests_common

USEMODULE += uuid
USEMODULE += suit_v4_cbor
USEPKG += tinycbor
USEMODULE += riotboot_slot riotboot_hdr
FEATURES_REQUIRED += riotboot

#manifest.cbor: manifest.yaml
#	python yaml2cbor.py $< $@

manifest.cbor.h: manifest.cbor
	xxd -i $< $@

all: manifest.cbor.h

include $(RIOTBASE)/Makefile.include
