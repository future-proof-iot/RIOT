# Overview

This a test application for suit module. The application sets-up a node with all
the software needed to perform suit-updates. Currently it only works for wireless
nodes so you need to setup a border router.

## Requirements

Dependencies:

    Python3 : - python > 3.6
              - ed25519 > 1.4
              - pyasn1  > 0.4.5
              - cbor    > 1.0.0 
              - aiocoap > 0.4
              - Click   > 7.0

When this was implemented aiocoap > 0.4 must be built from source you can follow 
installation instructions here https://aiocoap.readthedocs.io/en/latest/installation.html.

## Setup

### Key Generation

Generate key:

    $ BOARD=samr21-xpro make suit/genkey

Generate key header:

    $ BOARD=samr21-xpro make suit/keyhdr

### Set-up File server

Start aiocoap-fileserver:

    $ mkdir ${RIOTBASE}/coaproot
    $ <PATH>/aiocoap-fileserver ${RIOTBASE}/coaproot

If aiocoap was cloned and built from source aiocoap-filserver will be located
at <AIOCOAP_BASE_DIR>/aiocoap/contrib.

### Setup a BR

On a different board flash follow the instruction and setup [gnrc_border_router](https://github.com/RIOT-OS/RIOT/tree/master/examples/gnrc_border_router).

Pay attention to the prefix you will use, if you follow the border router instructions
it would be '2001:db8::/64' when running:

    $ sudo sh start_network.sh /dev/ttyACMx tap0 2001:db8::1/64

Add a routable address to host:

    $ sudo ip address add 2001:db8::1/128 dev tap0

### Provision IoT device (initial flash)

    $ BOARD=samr21-xpro make clean riotboot/flash -j4

### Publish 2 versions of firmware

    $ SUIT_EXTENSION=late-1 SUIT_COAP_SERVER=[fd00:dead:beef::1] BOARD=samr21-xpro suit/publish
    $ SUIT_EXTENSION=late-2 SUIT_COAP_SERVER=[fd00:dead:beef::1] BOARD=samr21-xpro suit/publish

## Test

Once all the setup is completed you can launch automatic test. It will notify the node to update
twice and will verify it is stil updatable when dooing so.

    $ BOARD=samr21-xpro make test
