#!/usr/bin/env python3

# Copyright (C) 2019 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import signal
import subprocess
import sys
import time
from testrunner import run

UPDATE_TIMEOUT = 120
MANIFEST_TIMEOUT = 15

CURRENT_DIR = os.path.dirname(sys.argv[0])
RIOT_DIR = os.path.abspath(os.path.join(CURRENT_DIR, '../../..'))
SUIT_DIR = os.path.abspath(os.path.join(RIOT_DIR, 'dist/tools/suit/'))

SUIT_LATEST_1 = 'late-1'
SUIT_LATEST_2 = 'late-2'

COAP_HOST = '[fd00:dead:beef::1]'


def notify(server_url, client_url, latest):
    cmd = ['make', 'SUIT_EXTENSION={}'.format(latest), 'suit/notify',
           'SUIT_COAP_SERVER={}'.format(server_url),
           'SUIT_CLIENT={}'.format(client_url)]
    subprocess.Popen(cmd).wait()


def testfunc(child):
    """For one board test if specified application is updatable"""

    # Intial Setup and wait for address configuration
    time.sleep(3)
    child.expect_exact('main(): This is RIOT!')
    child.expect(r'inet6 addr: (?P<gladdr>[0-9a-fA-F:]+:[A-Fa-f:0-9]+)'
                 '  scope: global  VAL')
    client = "[{}]".format(child.match.group("gladdr").lower())

    # Notify first time
    notify(COAP_HOST, client, SUIT_LATEST_1)
    child.expect_exact('suit_coap: trigger received')
    child.expect_exact('suit: verifying manifest signature...')
    child.expect(
        r'riotboot_flashwrite: initializing update to target slot \d',
        timeout=MANIFEST_TIMEOUT)

    # Wait for update to complete
    child.expect_exact(
        'riotboot_flashwrite: riotboot flashing completed successfully',
        timeout=UPDATE_TIMEOUT)
    child.expect_exact('main(): This is RIOT!')

    # Notify second time
    notify(COAP_HOST, client, SUIT_LATEST_2)
    child.expect_exact('suit_coap: trigger received')
    child.expect_exact('suit: verifying manifest signature...')
    child.expect(
        r'riotboot_flashwrite: initializing update to target slot \d',
        timeout=MANIFEST_TIMEOUT)

    # Wait for update to complete
    child.expect_exact(
        'riotboot_flashwrite: riotboot flashing completed successfully',
        timeout=UPDATE_TIMEOUT)
    child.expect_exact('main(): This is RIOT!')
    print("TEST PASSED")

if __name__ == "__main__":
    sys.exit(run(testfunc, echo=True))
