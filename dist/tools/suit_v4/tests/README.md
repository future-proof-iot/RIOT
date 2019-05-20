# Suit Tests Script

This helper script allows setting up some of the requirements needed by 
[tests/suit_update](https://github.com/future-proof-iot/RIOT/tree/master/tests/suit_update).

If no options are passed the script wont do anything.

## Prerequirement

Dependencies:
    
    Python3 : - python > 3.6
              - ed25519 > 1.4
              - pyasn1  > 0.4.5
              - cbor    > 1.0.0 
              - aiocoap > 0.4
              - Click   > 7.0

If a BorderRouter is to be setup than the requirements are:

    - Board flashed with [gnrc_border_router](https://github.com/future-proof-iot/RIOT/tree/master/examples/gnrc_border_router)

## Usage

Running the script:

    $ python suit_test_setup.py app_dir <options>

- app_dir = directory of application to test

Options:

- Use `--board-node` option to specify the target board, (default samr21-xpro)
- Use `--board-node` option to specify the BR board, (default iotlab-m3)
- Use `--coap-host` option to specify the CoAP server IP that will be used by
  the devices to fetch the files (`fd00:dead:beef::1` is the default)
- Use `--ethos` setup the BR and propagate a prefix, (default False)
- Use `--fileserver` setup aiocoap-fileserver, (default False)
- Use `--jobs` Parallel building (0 means not limit, like '--jobs')
- Use `--keys` create new keys, (default False)
- Use `--loglevel` option to specify debug logs.
- Use `--make` Additional make arguments, default none
- Use `--node` Provision the target, flash the node (Default False)
- Use `--port-ethos` Serial port for border router, (`/dev/ttyUSB1` is default)
- Use `--port-node` Serial port for target node, (`/dev/ttyACM0` is default)
- Use `--prefix` Prefix to propagate over ethos (`fd01::1/64` is default)
- Use `--publish` Publish new firmware (Default False)
- Use `--riot_dir` Riot base directory
- Use `--tags` String list of tag under which to publish latest manifest link (Default latest)
- Use `--test` Run Make test on target application (Default False)
