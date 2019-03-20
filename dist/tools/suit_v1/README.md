### Suit V1 Manifest Generation

This tool can be used to generate a manifest according [draft-moran-suit-manifest-01](https://tools.ietf.org/html/draft-moran-suit-manifest-01).

#### Installation

V1 Manifest Generator requires Python 3 (>= 3.5).
All requirements can be installed with [pip](https://github.com/pypa/pip).

0. When using python applications it is usually sugested to use virtual
environments. You can find instructions to set this up [here](https://virtualenvwrapper.readthedocs.io/en/latest/install.html) or [here](https://virtualenv.pypa.io/en/latest/installation/). If you do any of those
options you can skip 1 and just set up a virtual env for this project.

1. Install pip for Python 3 using:

    $ wget -qO - https://bootstrap.pypa.io/get-pip.py | python3

2. Install the manifest generator dependecies:

    $ cd RIOT/dist/tools/suit_v1
    $ pip3 install -r requirements.txt

#### Generate a Manifest

Generate a manifest by running:

    $ python $(RIOTBASE)/dist/tools/suit_v1/gen_manifest.py

Parameters Required :

- Use `-u` str, full uri to the binary
- Use `-k` file containing the ed25519, can be in raw format or ASN.1
    - Use `--raw` if key file is a raw ed25519 byte key
- Use `-c` str, device class
- Use `-e` str, vendor name 
- Use `-d` str, device ID uuid
- Use `-V` int, representing firmware version
- Use `-b` int, manifest valid duration in seconds
- Use `-o` file, signed manifest output file
- Use `-file` to indicate vendor name


### NOTES

Parameter not present in manifest (representend as nil)
    * text
    * directives
    * aliases
    * dependencies
    * extensions

Sha256 is used for the digest.