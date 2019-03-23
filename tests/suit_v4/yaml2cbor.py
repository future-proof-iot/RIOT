import yaml
import cbor
import sys

with open(sys.argv[1], "rb") as infile:
    with open(sys.argv[2], "wb") as outfile:
        data = yaml.load(infile)
        print(data)
        cbor.dump(data, outfile, sort_keys=True)
