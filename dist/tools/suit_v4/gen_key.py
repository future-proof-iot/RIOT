#!/usr/bin/env python3

import ed25519
import sys
signing_key, verifying_key = ed25519.create_keypair()
open(sys.argv[1], "wb").write(signing_key.to_bytes())
vkey_hex = verifying_key.to_ascii(encoding="hex")
open(sys.argv[2], "wb").write(verifying_key.to_bytes())
print("the public key is", vkey_hex)
