# Overview

This example application shows how to integrate SUIT software updates into a
RIOT application. This application will only be implementing basic v1 support of
the [draft-moran-suit-manifest-00](https://datatracker.ietf.org/doc/draft-moran-suit-manifest/01/).

## Prerequisites

Dependencies:
    
    Python3 : - ed25519 > 1.4
              - pyasn1  > 0.4.5
              - cbor    > 1.0.0 
              - aiocoap > 0.4

When this was implemented aiocoap > 0.4 must be built from source you can follow 
installation instructions here https://aiocoap.readthedocs.io/en/latest/installation.html.
If you don't choose to clone the repo locally you still need to download "aiocoap-filesever"
 from https://github.com/chrysn/aiocoap/blob/master/contrib/aiocoap-fileserver.

- RIOT repository checked out into $RIOTBASE

(*) cbor is installed as a dependency of aiocoap but is needed on its own if another
server is used.

# Setup

### Initial flash

In order to get a SUIT capable firmware onto the node. In examples/suit_update:

    $ BOARD=samr21-xpro make  clean riotboot/flash -j4

### Setup network

First, you need to compile `ethos`.
Go to `/dist/tools/ethos` and type:

    $ make clean all

Then, you need to compile UHCP.
This tool is found in `/dist/tools/uhcpd`. So, as for `ethos`:

    $ make clean all

In one shell and with the board already flashed and connected to /dev/ttyACM0:

    $ cd $RIOTBASE/dist/tools/ethos
    $ sudo ./start_network.sh /dev/ttyACM0 riot0 fd00::1/64

Once everyhting is configured you will get:

    ...

    Iface  7  HWaddr: 00:22:09:17:DD:59
            L2-PDU:1500 MTU:1500  HL:64  RTR
            Source address length: 6
            Link type: wired
            inet6 addr: fe80::222:9ff:fe17:dd59  scope: local  TNT[1]
            inet6 addr: fe80::2  scope: local  VAL
            inet6 group: ff02::2
            inet6 group: ff02::1
            inet6 group: ff02::1:ff17:dd59
            inet6 group: ff02::1:ff00:2

    suit_coap: started.

Keep this running (don't close the shell).

Add a routable address to host:

    $ sudo ip address add fd01::1/128 dev riot0

Start aiocoap-fileserver:

    $ mkdir ${RIOTBASE}/coaproot
    $ <PATH>/aiocoap-fileserver ${RIOTBASE}/coaproot

If aiocoap was cloned and built from source aiocoap-filserver will be located
at <AIOCOAP_BASE_DIR>/aiocoap/contrib.

### Key Generation

To sign the manifest a key must be generated.

In examples/suit_update:

    $ BOARD=samr21-xpro make suit/genkey

You will get this message in the terminal:

    the public key is b'a0fc7fe714d0c81edccc50c9e3d9e6f9c72cc68c28990f235ede38e4553b4724'

### Publish

Currently, the build system assumes that it can publish files by simply copying
them to a configurable folder. For this example, aiocoap-fileserver will then
serve the files via CoAP.

Manifests and image files will be copied to
$(SUIT_COAP_FSROOT)/$(SUIT_COAP_BASEPATH).

In examples/suit_update:

    $ BOARD=samr21-xpro SUIT_COAP_SERVER=[fd01::1] make suit/publish

This will publish into the server new firmware for a samr21-xpro board. You should
see 6 pairs of messages indicating where (filepath) the file was published and
the coap resource URI

    ...
    published "/home/francisco/workspace/RIOT/examples/suit_update/bin/samr21-xpro/suit_update-slot0.riot.suit.1553182001.bin"
        as "coap://[fd01::1]/fw/samr21-xpro/suit_update-slot0.riot.suit.1553182001.bin"
    ...


### Notify node

If the network has been started as described above, the RIOT node should be
reachable via link-local "fe80::2" on the ethos interface.

    $ SUIT_COAP_SERVER='[fd01::1]' SUIT_CLIENT=[fe80::2%riot0] BOARD=samr21-xpro make suit/notify

This will notify the node of new available manifest and it will fetch it. It will
take some time to fetch and write to flash, you will a series of messages like:

    ....
    riotboot_flashwrite: processing bytes 1344-1407
    riotboot_flashwrite: processing bytes 1408-1471
    riotboot_flashwrite: processing bytes 1472-1535
    ...

Once the new image is written, the device will reboot and display:

    main(): This is RIOT! (Version: 2019.04-devel-606-gaa7b-ota_suit_v2)
    RIOT SUIT update example application
    running from slot 1
    Waiting for address autoconfiguration...

The slot number should have changed form when you started the application.
You can do the publish-notify sequence again to verify this.

# In depth explanation

## Node

For the suit_update to work there are important block that aren't normally built
in a RIOT application:

* riotboot
    * riotboot_hdr
* riotboot_slot
* suit
    * suit_coap
    * suit_v1

### riotboot

To be able to receive updates, the firmware on the device needs a bootloader
that can decide from witch of the firmware images (new one and olds ones) to boot.

For suit updates you need at least two slots in the current conception on riotboot.
The flash memory will be divided in the following way:

```
|------------------------------- FLASH ------------------------------------------------------------|
|-RIOTBOOT_LEN-|------ RIOTBOOT_SLOT_SIZE (slot 0) ------|------ RIOTBOOT_SLOT_SIZE (slot 1) ------|
               |----- RIOTBOOT_HDR_LEN ------|           |----- RIOTBOOT_HDR_LEN ------|
 --------------------------------------------------------------------------------------------------|
|   riotboot   | riotboot_hdr_1 + filler (0) | slot_0_fw | riotboot_hdr_2 + filler (0) | slot_1_fw |
 --------------------------------------------------------------------------------------------------|
```

The riotboot part of the flash will not be changed during suit_updates but
be flashed a first time with at least one slot with suit_capable fw.

    $ BOARD=samr21-xpro make -C examples/suit_update clean riotboot/flash

When calling make with the riotboot/flash argument it will flash the bootloader
and then to slot0 a copy of the firmware you intend to build.

New images must be of course written to the inactive slot, the device mist be able
to boot from the previous image in case the update had some kind of error, eg:
the image corresponds to the wrong slot.

The active/inactive coap resources is used so the publisher can send a manifest
built for the inactive slot.

On boot the bootloader will check the riotboot_hdr and boot on the newest
image.

riotboot is not supported by all boards. Current supported boards are:

* saml21-xpro
* samr21-xpro
* nucleo-l152re
* iotlab-a8
* iotlab-m3
* nrfxxx


### suit

The suit module encloses all the other suit_related module. Formally this only
includes the sys/suit directory into the build system dirs.

#### suit_coap

To enable support for suit_updates over coap a new thread is created.
This thread will expose 4 suit related resources:
-=
* /suit/slot/active: a resource that returns the number of their active slot
* /suit/slot/inactive: a resource that returns the number of their inactive slot
* /suit/trigger: this resource allows POST/PUT where the payload is assumed
tu be a url with the location of a manifest for a new firmware update on the
inactive slot.
* /suit/version: this resource is currently not implemented and return "NONE",
it should return the version of the application running on the device.

When a new manifest url is received on the trigger resource a message is resent
to the coap thread with the manifest's url. The thread will then fetch the
manifest by a block coap request to the specified url.

If suit_v1 is included the manifest will also be parsed.

#### suit_v1

This includes v1 manifest support. When a url is received in the /suit/trigger
coap resource it will trigger a coap blockwise fetch of the manifest. When this
manifest is received it will be parsed. If the received manifest is valid it
will extract the url for the firmware location from the manifest.

It will then fetch the firmware, write it to the inactive slot and reboot the device.
From there the bootloader takes over, verifying the slot riotboot_hdr and boots
from the newest image.

#### pending v1 support

Once the firmware location url is parsed it should fetch the firmware and copy it
to the inactive slot. After copying ti should reboot so the device can boot
from the new firmware image. This is handled by riotboot module.

#### support for v4

To support v4 most of the infrastructure is kept the same, you would have
to implement handling of a v4 manifest and add it to _suit_handle_url() in
sys/suit/coap.c.

## Network

For connecting the device with the internet we are using ethos (a simple
ethernet over serial driver).

When executing $RIOTBASE/dist/tools/ethos:

    $ sudo ./start_network.sh /dev/ttyACM0 riot0 fd00::1/64

A tap interface named "riot0" is setup. fe80::1/64 is set up as it's
link local address and fd00:dead:beef::1/128 as the "lo" unique link local address.

Also fd00::1/64 is configured- as a prefix for the network. It also sets-up
a route to the fd00::1/64 subnet threw fe80::2. Where fe80::2 is the default
link local address of the UHCP interface.

Finally when:

    $ sudo ip address add fd01::1/128 dev riot0

We are adding a routable address to the riot0 tap interface. The device can
now send messages to the the coap server threw the riot0 tap interface. You could
use a different address for the coap server as long as you also add a routable
address, so:

    $ sudo ip address add $(SUIT_COAP_SERVER) dev riot0

NOTE: if we weren't using a local server you would need to have ipv6 support
on your network or use tunneling.

NOTE: using fd00:dead:beef::1 as an address for the coap server would also
work and you would;t need to add a routable address to the tap interface since
a route to the loopback interface (lo) is already configured.

## Sever and File System Variables

The following variables are defined in makefiles/suit.inc.mk:

    SUIT_COAP_BASEPATH ?= firmware/$(APPLICATION)/$(BOARD)
    SUIT_COAP_SERVER ?= localhost
    SUIT_COAP_ROOT ?= coap://$(SUIT_COAP_SERVER)/$(SUIT_COAP_BASEPATH)
    SUIT_COAP_FSROOT ?= $(RIOTBASE)/coaproot

The following convention is used when naming a manifest

    SLOTx_SUIT_MANIFEST ?= $(BINDIR_APP)-slotx.riot.suit.$(APP_VER).bin
    SLOTx_SUIT_MANIFEST_LATEST ?= $(BINDIR_APP)-slotx.riot.suit.latest.bin

The following default values are using for generating the manifest:

    SUIT_VENDOR ?= RIOT
    SUIT_VERSION ?= $(APP_VER)
    SUIT_DEVICE_ID ?= $(BOARD)
    SUIT_KEY ?= secret.key

All files (both slot binaries, both manifests, copies of manifests with
"latest" instead of $APP_VER in riotboot build) are copied into the folder
$(SUIT_COAP_FSROOT)/$(SUIT_COAP_BASEPATH). The manifests contain URLs to
$(SUIT_COAP_ROOT)/* and are signed that way.

The whole tree under $(SUIT_COAP_FSROOT) is expected to be served via CoAP
under $(SUIT_COAP_ROOT). This can be done by e.g., "aiocoap-fileserver $(SUIT_COAP_FSROOT)".

## Makefile recipes

The following recipes are defined in makefiles/suit.inc.mk

suit/manifest: creates manifest for all slots and "latest" tag of each,
    uses following parameters:

    - $(SUIT_KEY): key to sign the manifest
	- $(SUIT_COAP_ROOT): coap root address
	- $(SUIT_DEVICE_ID)
	- $(SUIT_VERSION)
	- $(SUIT_VENDOR)

suit/publish: makes the suit manifest, slotx bin and publishes it to the
    aiocoap-fileserver

    1.- builds slot0 and slot1 bin's
    2.- builds manifest
    3.- creates $(SUIT_COAP_FSROOT)/$(SUIT_COAP_BASEPATH) directory
    4.- copy's binaries to $(SUIT_COAP_FSROOT)/$(SUIT_COAP_BASEPATH)
    - $(SUIT_COAP_ROOT): root url for the coap resources

suit/notify: triggers a device update, it sends two requests:

    1.- COAP get to check which slot is inactive on the device
    2.- COAP POST with the url where to fetch the latest manifest for
    the inactive slot

    - $(SUIT_CLIENT): define the client ipv6 address
    - $(SUIT_COAP_ROOT): root url for the coap resources
    - $(SLOTx_SUIT_MANIFEST_LATEST): name of the resource where the slotx
    manifest is exposed.

suit/genkey: this recipe generates a ed25519 key to sign the manifest

**NOTE: to plugin a new server you would only have to change the suit/publish
recipe, respecting or adjusting to the naming conventions.**
