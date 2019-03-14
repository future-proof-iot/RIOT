#
SUIT_COAP_BASEPATH ?= firmware/$(APPLICATION)/$(BOARD)
SUIT_COAP_ROOT ?= coap://foo:bar::1/$(SUIT_COAP_BASEPATH)

#
SLOT0_SUIT_MANIFEST ?= $(BINDIR_APP)-slot0.riot.suit.$(APP_VER).bin
SLOT1_SUIT_MANIFEST ?= $(BINDIR_APP)-slot1.riot.suit.$(APP_VER).bin

$(SLOT0_SUIT_MANIFEST): $(SLOT0_RIOT_BIN)
	@echo "$(@): manifest generation not implemented!"

$(SLOT1_SUIT_MANIFEST): $(SLOT1_RIOT_BIN)
	@echo "$(@): manifest generation not implemented!"

suit/manifest: $(SLOT0_SUIT_MANIFEST) $(SLOT1_SUIT_MANIFEST)

suit/publish: suit/manifest
	@echo "$(@): publish not implemented!"
	#coap-server-publish $(SLOT0_RIOT_BIN) $(SUIT_COAP_ROOT)/whatever
	#coap-server-publish $(SLOT0_SUIT_MANIFEST) $(SUIT_COAP_ROOT)/whatever
