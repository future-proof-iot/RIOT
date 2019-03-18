#
SUIT_COAP_BASEPATH ?= firmware/$(APPLICATION)/$(BOARD)
SUIT_COAP_ROOT ?= coap://foo:bar::1/$(SUIT_COAP_BASEPATH)

#
SLOT0_SUIT_MANIFEST ?= $(BINDIR_APP)-slot0.riot.suit.$(APP_VER).bin
SLOT1_SUIT_MANIFEST ?= $(BINDIR_APP)-slot1.riot.suit.$(APP_VER).bin

SUIT_VENDOR ?= RIOT
SUIT_VERSION ?= $(APP_VER)
SUIT_DEVICE_ID ?= $(BOARD)
SUIT_KEY ?= secret.key

define manifest-recipe
  $(RIOTBASE)/dist/tools/suit_v1/gen_manifest.py \
	  --raw -k $(SUIT_KEY) \
	  -u $(SUIT_COAP_ROOT)/$(notdir $<) \
	  -d $(SUIT_DEVICE_ID) \
	  -V $(SUIT_VERSION) \
	  -e $(SUIT_VENDOR) \
	  -o $@ \
	  $<
endef

$(SLOT0_SUIT_MANIFEST): $(SLOT0_RIOT_BIN) $(SUIT_KEY)
	$(manifest-recipe)

$(SLOT1_SUIT_MANIFEST): $(SLOT1_RIOT_BIN) $(SUIT_KEY)
	$(manifest-recipe)

suit/manifest: $(SLOT0_SUIT_MANIFEST) $(SLOT1_SUIT_MANIFEST)

suit/publish: suit/manifest
	@echo "$(@): publish not implemented!"
	#coap-server-publish $(SLOT0_RIOT_BIN) $(SUIT_COAP_ROOT)/whatever
	#coap-server-publish $(SLOT0_SUIT_MANIFEST) $(SUIT_COAP_ROOT)/whatever

suit/genkey:
	$(RIOTBASE)/dist/tools/suit_v1/gen_key.py
