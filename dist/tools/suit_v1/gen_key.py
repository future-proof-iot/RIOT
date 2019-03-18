#!/usr/bin/env python3

import ed25519
signing_key, verifying_key = ed25519.create_keypair()
open("secret.key", "wb").write(signing_key.to_bytes())
vkey_hex = verifying_key.to_ascii(encoding="hex")
print("the public key is", vkey_hex)
