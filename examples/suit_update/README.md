# Overview

This example shows how to integrate SUIT-compliant firmware updates into a
RIOT application. It implements basic support of the SUIT architecture using
the manifest format specified in
[draft-moran-suit-manifest-04](https://datatracker.ietf.org/doc/draft-moran-suit-manifest/04/).

Table of contents:

- [Prerequisites][prerequisites]
- [Setup][setup]
  - [Sign key generation][key-generation]
  - [Setup a wired device using ethos][setup-wired]
    - [Provision the device][setup-wired-provision]
    - [Configure the network][setup-wired-network]
  - [Setup a wireless device behind a border router][setup-wireless]
    - [Provision the wireless device][setup-wireless-provision]
    - [Configure the wireless network][setup-wireless-network]
  - [Setup aiocoap fileserver][setup-aiocoap-fileserver]
- [Perform an update][update]
  - [Build and publish the firmware update][update-build-publish]
  - [Notify an update to the device][update-notify]
- [Detailed explanation][detailed-explanation]

## Prerequisites
[prerequisites]: #Prerequisites

- Install python dependencies (only Python3.6 and later is supported):

      $ sudo pip install ed25519 pyasn1 cbor click

- Install aiocoap from the source

      $ sudo pip3 install --upgrade "git+https://github.com/chrysn/aiocoap#egg=aiocoap[all]"

  See the [aiocoap installation instructions](https://aiocoap.readthedocs.io/en/latest/installation.html)
  for more details.

- Clone this repository:

      $ git clone https://github.com/future-proof-iot/RIOT -b suit
      $ cd RIOT

- In all setup below, `ethos` (EThernet Over Serial) is used to provide an IP
  link between the host computer and a board.

  Just build `ethos` and `uhcpd` with the following commands:

      $ make -C dist/tools/ethos clean all
      $ make -C dist/tools/uhcpd clean all

## Setup
[setup]: #Setup

### Key Generation
[key-generation]: #Key-generation

To sign the manifest and for de the device to verify the manifest a key must be generated.

Simply use the `suit/genkey` make target:

    $ BOARD=samr21-xpro make -C examples/suit_update suit/genkey

You will get this message in the terminal:

    the public key is b'a0fc7fe714d0c81edccc50c9e3d9e6f9c72cc68c28990f235ede38e4553b4724'

This also generates the `public_key.h` that will be included in the built firmware.

### Setup a wired device using ethos
[setup-wired]: #Setup-a-wired-device-using-ethos

#### Provision the device
[setup-wired-provision]: #Provision-the-device

In order to get a SUIT capable firmware onto the node. In examples/suit_update:

    $ BOARD=samr21-xpro make -C examples/suit_update clean riotboot/flash -j4

#### Configure the network
[setup-wired-network]: #Configure-the-network

In one terminal and with the board already flashed and connected to /dev/ttyACM0:

    $ sudo ./dist/tools/ethos/start_network.sh /dev/ttyACM0 tap0 2001:db8::/64

Once everything is configured you will get:

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

From another terminal, add a routable address to host:

    $ sudo ip address add 2001:db8::1/128 dev tap0

If the network has been started as described above, the RIOT updatable device
is reachable on its link-local address `fe80::2` via the `tap0` interface but
not when using its wireless interface global address:

    $ ping6 fe80::2%tap0

### Setup a wireless device behind a border router
[setup-wireless]: #Setup-a-wireless-device-behind-a-border-router

#### Configure the wireless network
[setup-wireless-network]: #Configure-the-wireless-network

A wireless node has no direct connection to the Internet so a border router (BR)
between 802.15.4 and Ethernet must be configured.
Any board providing a 802.15.4 radio can be used as BR.

Plug the BR board on the computer and flash the
[gnrc_border_router](https://github.com/RIOT-OS/RIOT/tree/master/examples/gnrc_border_router)
application on it:

    $ make BOARD=<BR board> -C examples/gnrc_border_router flash

In on terminal, start the network (assuming on the host the virtual port of the
board is `/dev/ttyACM0`):

    $ sudo ./dist/tools/ethos/start_network.sh /dev/ttyACM0 tap0 2001:db8::/64

Keep this terminal open.

From another terminal on the host, add a routable address on the host `tap0`
interface:

    $ sudo ip address add 2001:db8::1/128 dev tap0

#### Provision the wireless device
[setup-wireless-provision]: #Provision-the-wireless-device

In this scenario the node will be connected through a border router. Ethos must
be disabled in the firmware when building and flashing the firmware:

    $ USE_ETHOS=0 BOARD=samr21-xpro make -C examples/suit_update clean riotboot/flash -j4

Open a serial terminal on the device to get its global address:

    $ USE_ETHOS=0 BOARD=samr21-xpro make -C examples/suit_update term

If the Border Router is already set up when opening the terminal you should get

    ...

    Iface  6  HWaddr: 0D:96  Channel: 26  Page: 0  NID: 0x23
            Long HWaddr: 79:7E:32:55:13:13:8D:96
             TX-Power: 0dBm  State: IDLE  max. Retrans.: 3  CSMA Retries: 4
            AUTOACK  ACK_REQ  CSMA  L2-PDU:102 MTU:1280  HL:64  RTR
            RTR_ADV  6LO  IPHC
            Source address length: 8
            Link type: wireless
            inet6 addr: fe80::7b7e:3255:1313:8d96  scope: local  VAL
            inet6 addr: 2001:db8::7b7e:3255:1313:8d96  scope: global  VAL
            inet6 group: ff02::2
            inet6 group: ff02::1
            inet6 group: ff02::1:ff17:dd59
            inet6 group: ff02::1:ff00:2

    suit_coap: started.

Here the global IPv6 is `2001:db8::7b7e:3255:1313:8d96`.
**The address will be different according to your device and the chosen prefix**.
In this case the RIOT node can be reached from the host using its global address:

    $ ping6 2001:db8::7b7e:3255:1313:8d96

### Setup aiocoap-fileserver
[setup-aiocoap-fileserver]: #Setup-aiocoap-fileserver

`aiocoap-fileserver` is used for hosting firmwares available for updates. Device
retrieve the new firmware using the CoAP protocol.

Clone the aiocoap repository:

    $ git clone https://github.com/chrysn/aiocoap.git ..

Start `aiocoap-fileserver`:

    $ mkdir -p coaproot
    $ ../aiocoap/contrib/aiocoap-fileserver coaproot

Keep the server running in the terminal.

## Perform an update
[update]: #Perform-an-update

### Build and publish the firmware update
[update-build-publish]: #Build-and-publish-the-firmware-update

Currently, the build system assumes that it can publish files by simply copying
them to a configurable folder.

For this example, aiocoap-fileserver serves the files via CoAP.

- To publish an update for a node in wired mode (behind ethos):

      $ BOARD=samr21-xpro SUIT_COAP_SERVER=[2001:db8::1] make -C examples/suit_update suit/publish

- To publish an update for a node in wireless mode (behind a border router):

      $ BOARD=samr21-xpro USE_ETHOS=0 SUIT_COAP_SERVER=[2001:db8::1] make -C examples/suit_update suit/publish

This publishes into the server a new firmware for a samr21-xpro board. You should
see 6 pairs of messages indicating where (filepath) the file was published and
the corresponding coap resource URI

    ...
    published "/home/francisco/workspace/RIOT/examples/suit_update/bin/samr21-xpro/suit_update-riot.suitv4_signed.1557135946.bin"
           as "coap://[2001:db8::1]/fw/samr21-xpro/suit_update-riot.suitv4_signed.1557135946.bin"
    published "/home/francisco/workspace/RIOT/examples/suit_update/bin/samr21-xpro/suit_update-riot.suitv4_signed.latest.bin"
           as "coap://[2001:db8::1]/fw/samr21-xpro/suit_update-riot.suitv4_signed.latest.bin"
    ...

### Notify an update to the device
[update-notify]: #Norify-an-update-to-the-device

If the network has been started with a standalone node, the RIOT node should be
reachable via link-local "fe80::2%tap0" on the ethos interface. If it was setup as a
wireless device it will be reachable via its global address, something like "2001:db8::7b7e:3255:1313:8d96"

- In wired mode:

      $ SUIT_COAP_SERVER=[2001:db8::1] SUIT_CLIENT=[fe80::2%tap0] BOARD=samr21-xpro make -C examples/suit_update suit/notify

- In wireless mode:

      $ SUIT_COAP_SERVER=[2001:db8::1] SUIT_CLIENT=[2001:db8::7b7e:3255:1313:8d96] BOARD=samr21-xpro make -C examples/suit_update suit/notify


This notifies the node of a new available manifest. Once the notification is
received by the device, it fetches it.

If using suit-v4 the node hangs for a couple of seconds when verifying the
signature:

    ....
    INFO # suit_coap: got manifest with size 545
    INFO # jumping into map
    INFO # )got key val=1
    INFO # handler res=0
    INFO # got key val=2
    INFO # suit: verifying manifest signature...
    ....

Once the signature is validated it continues validating other parts of the
manifest.
Among these validations it checks some condition like firmware offset position
in regards to the running slot to see witch firmware image to fetch.

    ....
    INFO # Handling handler with key 10 at 0x2b981
    INFO # Comparing manifest offset 4096 with other slot offset 4096
    ....
    INFO # Handling handler with key 10 at 0x2b981
    INFO # Comparing manifest offset 133120 with other slot offset 4096
    INFO # Sequence handler error
    ....

Once the manifest validation is complete, the application fetches the image
and starts flashing.
This step takes some time to fetch and write to flash, a series of messages like
the following are printed to the terminal:

    ....
    riotboot_flashwrite: processing bytes 1344-1407
    riotboot_flashwrite: processing bytes 1408-1471
    riotboot_flashwrite: processing bytes 1472-1535
    ...

Once the new image is written, a final validation is performed and, in case of
success, the application reboots on the new slot:

    2019-04-05 16:19:26,363 - INFO # riotboot: verifying digest at 0x20003f37 (img at: 0x20800 size: 80212)
    2019-04-05 16:19:26,704 - INFO # handler res=0
    2019-04-05 16:19:26,705 - INFO # got key val=10
    2019-04-05 16:19:26,707 - INFO # no handler found
    2019-04-05 16:19:26,708 - INFO # got key val=12
    2019-04-05 16:19:26,709 - INFO # no handler found
    2019-04-05 16:19:26,711 - INFO # handler res=0
    2019-04-05 16:19:26,713 - INFO # suit_v4_parse() success
    2019-04-05 16:19:26,715 - INFO # SUIT policy check OK.
    2019-04-05 16:19:26,718 - INFO # suit_coap: finalizing image flash
    2019-04-05 16:19:26,725 - INFO # riotboot_flashwrite: riotboot flashing completed successfully
    2019-04-05 16:19:26,728 - INFO # Image magic_number: 0x544f4952
    2019-04-05 16:19:26,730 - INFO # Image Version: 0x5ca76390
    2019-04-05 16:19:26,733 - INFO # Image start address: 0x00020900
    2019-04-05 16:19:26,738 - INFO # Header chksum: 0x13b466db


    main(): This is RIOT! (Version: 2019.04-devel-606-gaa7b-ota_suit_v2)
    RIOT SUIT update example application
    running from slot 1
    Waiting for address autoconfiguration...

The slot number should have changed from after the application reboots.
You can do the publish-notify sequence several times to verify this.

## Detailed explanation
[detailed-explanation]: #Detailed-explanation

### Node

For the suit_update to work there are important block that aren't normally built
in a RIOT application:

* riotboot
    * riotboot_hdr
* riotboot_slot
* suit
    * suit_coap
    * suit_v4

#### riotboot

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

#### suit

The suit module encloses all the other suit_related module. Formally this only
includes the sys/suit directory into the build system dirs.

- **suit_coap**

To enable support for suit_updates over coap a new thread is created.
This thread will expose 4 suit related resources:

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

- **support for v4**

This includes v4 manifest support. When a url is received in the /suit/trigger
coap resource it will trigger a coap blockwise fetch of the manifest. When this
manifest is received it will be parsed. The signature of the manifest will be
verified and then the rest of the manifest content. If the received manifest is valid it
will extract the url for the firmware location from the manifest.

It will then fetch the firmware, write it to the inactive slot and reboot the device.
Digest validation is done once all the firmware is written to flash.
From there the bootloader takes over, verifying the slot riotboot_hdr and boots
from the newest image.

### Network

For connecting the device with the internet we are using ethos (a simple
ethernet over serial driver).

When executing $RIOTBASE/dist/tools/ethos:

    $ sudo ./start_network.sh /dev/ttyACM0 tap0 2001:db8::1/64

A tap interface named "tap0" is setup. fe80::1/64 is set up as it's
link local address and fd00:dead:beef::1/128 as the "lo" unique link local address.

Also 2001:db8::1/64 is configured- as a prefix for the network. It also sets-up
a route to the 2001:db8::1/64 subnet through fe80::2. Where fe80::2 is the default
link local address of the UHCP interface.

Finally when:

    $ sudo ip address add 2001:db8::1/128 dev tap0

We are adding a routable address to the tap0 tap interface. The device can
now send messages to the the coap server through the tap0 tap interface. You could
use a different address for the coap server as long as you also add a routable
address, so:

    $ sudo ip address add $(SUIT_COAP_SERVER) dev tap0

When using a border router the same thing is happening although the node is no
longer reachable through its link local address but routed through to border router
so we can reach it with its global address.

NOTE: if we weren't using a local server you would need to have ipv6 support
on your network or use tunneling.

NOTE: using fd00:dead:beef::1 as an address for the coap server would also
work and you wouldn't need to add a routable address to the tap interface since
a route to the loopback interface (lo) is already configured.

### Server and file system variables

The following variables are defined in makefiles/suit.inc.mk:

    SUIT_COAP_BASEPATH ?= firmware/$(APPLICATION)/$(BOARD)
    SUIT_COAP_SERVER ?= localhost
    SUIT_COAP_ROOT ?= coap://$(SUIT_COAP_SERVER)/$(SUIT_COAP_BASEPATH)
    SUIT_COAP_FSROOT ?= $(RIOTBASE)/coaproot
    SUIT_PUB_HDR ?= public_key.h

The following convention is used when naming a manifest

    SUIT_MANIFEST ?= $(BINDIR_APP)-riot.suitv4.$(APP_VER).bin
    SUIT_MANIFEST_LATEST ?= $(BINDIR_APP)-riot.suitv4.latest.bin
    SUIT_MANIFEST_SIGNED ?= $(BINDIR_APP)-riot.suitv4_signed.$(APP_VER).bin
    SUIT_MANIFEST_SIGNED_LATEST ?= $(BINDIR_APP)-riot.suitv4_signed.latest.bin

The following default values are using for generating the manifest:

    SUIT_VENDOR ?= RIOT
    SUIT_VERSION ?= $(APP_VER)
    SUIT_DEVICE_ID ?= $(BOARD)
    SUIT_KEY ?= secret.key
    SUIT_PUB ?= public.key
    SUIT_PUB_HDR ?= public_key.h

All files (both slot binaries, both manifests, copies of manifests with
"latest" instead of $APP_VER in riotboot build) are copied into the folder
$(SUIT_COAP_FSROOT)/$(SUIT_COAP_BASEPATH). The manifests contain URLs to
$(SUIT_COAP_ROOT)/* and are signed that way.

The whole tree under $(SUIT_COAP_FSROOT) is expected to be served via CoAP
under $(SUIT_COAP_ROOT). This can be done by e.g., "aiocoap-fileserver $(SUIT_COAP_FSROOT)".

### Makefile recipes

The following recipes are defined in makefiles/suit.inc.mk

suit/manifest: creates a non signed and signed manifest, and also a latest tag for these.
    It uses following parameters:

    - $(SUIT_KEY): key to sign the manifest
    - $(SUIT_PUB): public key used to verify the manifest
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
    - $(SUIT_MANIFEST_SIGNED_LATEST): name of the resource where the latest signed
    manifest is exposed.

suit/genkey: this recipe generates a ed25519 key to sign the manifest

**NOTE**: to plugin a new server you would only have to change the suit/publish
recipe, respecting or adjusting to the naming conventions.**
