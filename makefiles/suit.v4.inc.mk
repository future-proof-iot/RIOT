#
SUIT_MAKEFILE ?= local

#
SUIT_MANIFEST ?= $(BINDIR_APP)-riot.suitv4.$(APP_VER).bin
SUIT_MANIFEST_SIGNED ?= $(BINDIR_APP)-riot.suitv4_signed.$(APP_VER).bin

# Long manifest names require more buffer space when parsing
export CFLAGS += -DSOCK_URLPATH_MAXLEN=128

SUIT_VENDOR ?= "riot-os.org"
SUIT_SEQNR ?= $(APP_VER)
SUIT_DEVICE_ID ?= $(BOARD)
SUIT_KEY ?= secret.key
SUIT_PUB ?= public.key
SUIT_PUB_HDR ?= public_key.h

ifeq (local,$(SUIT_MAKEFILE))
  include $(RIOTMAKE)/suit.v4.local.inc.mk
else
  include $(SUIT_MAKEFILE)
endif

$(SUIT_MANIFEST): $(SLOT0_RIOT_BIN) $(SLOT1_RIOT_BIN)
	$(RIOTBASE)/dist/tools/suit_v4/gen_manifest.py \
	  --template $(RIOTBASE)/dist/tools/suit_v4/test-2img.json \
	  --urlroot $(SUIT_COAP_ROOT) \
	  --seqnr $(SUIT_SEQNR) \
	  --uuid-vendor $(SUIT_VENDOR) \
	  --uuid-class $(SUIT_DEVICE_ID) \
	  --offsets $(SLOT0_OFFSET),$(SLOT1_OFFSET) \
	  -o $@ \
	  $^

$(SUIT_MANIFEST_SIGNED): $(SUIT_MANIFEST)
	$(RIOTBASE)/dist/tools/suit_v4/sign-04.py \
	  $(SUIT_KEY) $(SUIT_PUB) $< $@
