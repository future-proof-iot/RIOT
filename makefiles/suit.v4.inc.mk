#
SUIT_COAP_BASEPATH ?= fw/$(BOARD)
SUIT_COAP_SERVER ?= localhost
SUIT_COAP_ROOT ?= coap://$(SUIT_COAP_SERVER)/$(SUIT_COAP_BASEPATH)
SUIT_COAP_FSROOT ?= $(RIOTBASE)/coaproot

#
SUIT_MANIFEST ?= $(BINDIR_APP)-riot.suitv4.$(APP_VER).bin
SUIT_MANIFEST_LATEST ?= $(BINDIR_APP)-riot.suitv4.latest.bin
SUIT_MANIFEST_SIGNED ?= $(BINDIR_APP)-riot.suitv4_signed.$(APP_VER).bin
SUIT_MANIFEST_SIGNED_LATEST ?= $(BINDIR_APP)-riot.suitv4_signed.latest.bin

SUIT_VENDOR ?= "riot-os.org"
SUIT_SEQNR ?= $(APP_VER)
SUIT_DEVICE_ID ?= $(BOARD)
SUIT_KEY ?= secret.key
SUIT_PUB ?= public.key
SUIT_PUB_HDR ?= public_key.h

$(SUIT_PUB_HDR):
	xxd -i $(SUIT_PUB) > $@

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

$(SUIT_MANIFEST_LATEST): $(SUIT_MANIFEST)
	@ln -f -s $< $@

$(SUIT_MANIFEST_SIGNED_LATEST): $(SUIT_MANIFEST_SIGNED)
	@ln -f -s $< $@

SUIT_MANIFESTS := $(SUIT_MANIFEST) \
                  $(SUIT_MANIFEST_LATEST) \
		  $(SUIT_MANIFEST_SIGNED) \
		  $(SUIT_MANIFEST_SIGNED_LATEST)

suit/manifest: $(SUIT_MANIFESTS)

suit/publish: $(SUIT_MANIFESTS) $(SLOT0_RIOT_BIN) $(SLOT1_RIOT_BIN)
	@mkdir -p $(SUIT_COAP_FSROOT)/$(SUIT_COAP_BASEPATH)
	@cp -t $(SUIT_COAP_FSROOT)/$(SUIT_COAP_BASEPATH) $^
	@for file in $^; do \
		echo "published \"$$file\""; \
		echo "       as \"$(SUIT_COAP_ROOT)/$$(basename $$file)\""; \
	done

suit/notify: | $(filter suit/publish, $(MAKECMDGOALS))
	@test -n "$(SUIT_CLIENT)"
	aiocoap-client -m POST "coap://$(SUIT_CLIENT)/suit/trigger" \
		--payload "$(SUIT_COAP_ROOT)/$$(basename $(SUIT_MANIFEST_SIGNED_LATEST))" && \
		echo "Triggered $(SUIT_CLIENT) to update."

suit/genkey:
	$(RIOTBASE)/dist/tools/suit_v4/gen_key.py

suit/keyhdr: suit/genkey $(SUIT_PUB_HDR)
