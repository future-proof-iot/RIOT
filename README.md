<a href="https://sparta.eu/">
  <img alt="Sparta" src="https://www.sparta.eu/assets/images/sparta-logo-rectangle.png" width="400">
</a>

<a href="https://riot-os.org/">
  <img alt="RIOT" src="https://raw.githubusercontent.com/RIOT-OS/RIOT/master/doc/doxygen/src/riot-logo.svg" width="400">
</a>


[![Nightly CI status master][master-ci-badge]][master-ci-link]
[![GitHub release][release-badge]][release-link]
[![License][license-badge]][license-link]
[![API docs][api-badge]][api-link]

RIOT is an open source general-purpose operating system for low-power IoT devices,
which we co-founded, contribute continuously to.
For more information on RIOT, see the [master branch](https://github.com/RIOT-OS/RIOT), and our [prior publication](https://ilab-pub.imp.fu-berlin.de/papers/bghkl-rosos-18-prepub.pdf).

## H2020 Sparta D6.2 : Security-Enhanced IoT Operating System Software

We here showcase the output of our work in the context of D6.2 as open source
implementations and integrated with mainstream RIOT, which we forked in this repository.
The focus of this fork of RIOT (based on [Release 2020.10](https://github.com/RIOT-OS/RIOT/releases/tag/2020.10)) is to :

- highlight some of **our on-going contributions upstream to RIOT** master branch: hundreds of commits since the beginning of H2020 Sparta including, but not limited to, SUIT-compliant secure OS software update support, and support for secure 6TiSCH networking.
- offer a sneak peek at **complementary security mechanisms we developed** but that are not (yet) integrated in RIOT master branch, and how this platform is used in the context of Sparta, e.g. a prototype of minimal virtual machines for software module hosting and isolation.

Currently, among the features we mention below, only SUIT and secure 6TiSCH support have been merged into the master branch of RIOT. The other functionalities we mention are only present in this fork.

**Next Steps & Continuous Integration --** We plan to continue using a similar workflow with our upcoming contributions in the context of H2020 Sparta. We will upstream as much as possible our output to the master branch of RIOT, and in parallel, we will publish and maintain here (in this GitHub organization) complementary open source modules to showcase their potential integration.


## SUIT-compliant Secure IoT Software Updates

We co-author the SUIT standard proposed by IETF to secure IoT software updates. The [SUIT specifications](https://tools.ietf.org/html/draft-ietf-suit-manifest-09) specify a security architecture, and the necessary metadata and cryptography to secure software updates,
applicable on microcontroller-based devices, such as the ones RIOT runs on.
We integrate SUIT support for secure firmware updates as described in our [publication on this topic](https://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=8725488).
We support a SUIT-compliant workflow as depicted below, which is the blueprint for the AirMonitor demo, shown in a [video tutorial](https://github.com/future-proof-iot/RIOT/raw/H2020-Sparta-Deliverable-D6-2/videos/riot-suit.mp4) we provide.

To try this functionality on your own hardware (an nrf52840dk board for instance) start with the [suit_update example](examples/suit_update).

<a href="https://github.com/future-proof-iot/RIOT/tree/H2020-Sparta-Deliverable-D6-2">
  <img alt="SUIT" src="https://raw.githubusercontent.com/future-proof-iot/RIOT/H2020-Sparta-Deliverable-D6-2/doc/figures/SUIT-update-workflow.jpg" width="700">
</a>

As shown in the above figure, the workflow consists in a preliminary phase (Phase 0) whereby the maintainer produces and flashes the IoT devices with commissioning material : the bootloader, the initial image, and authorized crypto material. Once the IoT device commissioned, the maintainer can trigger iterations through cycles of phases 1-5, whereby the authorized maintainer can build a new image and sign the corresponding standard metadata (the SUIT manifest) which can be transferred to the device over the network via a repository (CoAP resource directory), and upon cryptographic verification on-board the device, the new image is installed and booted.

Using the mechanisms specified by SUIT, our security-enhanced OS can for example mitigate the below attacks.

*Tampered Firmware Update Attacks* –  An attacker may try to update the IoT device with a modified and intentionally flawed firmware image. To counter this threat, our prototype based on SUIT uses digital signatures on a hash of the image binary and the metadata to ensure integrity of both the firmware and its metadata. 

*Unauthorized Firmware Update Attacks* – An unauthorized party may attempt to update the IoT device with modified image. Using digital signatures and public key cryptography, our prototype based on SUIT ensure that only the authorized maintainer (holding the authorized private key) will be able to update de device.

*Firmware Update Replay Attacks* – An attacker may try to replay a valid, but old (known-to-be-flawed) firmware. This threat is mitigated by using a sequence number. Our prototype based on SUIT uses a sequence number, which is increased with every new firmware update.

*Firmware Update Mismatch Attacks* – An attacker may try replaying a firmware update that is authentic, but for an incompatible device. Our prototype based on SUIT includes device-specific conditions, which can be verified before installing a firmware image, thereby preventing the device from using an incompatible firmware image.

## Secure Low-power IoT Networking

We integrated OpenWSN, the standards-compliant open-source implementation of the [6TiSCH network stack](https://hal.inria.fr/hal-02420974/document), as described in our [publication on this topic](https://hal.inria.fr/hal-03064601/document).

To try out this functionality, start by trying out the steps described in the [dedicated documentation](http://doc.riot-os.org/group__pkg__openwsn.html),
which we also show in a [video tutorial](https://github.com/future-proof-iot/RIOT/raw/H2020-Sparta-Deliverable-D6-2/videos/riot-openwsn.mp4), which demonstrates secure network joining with CoJP ([Constrained Join Protocol](https://tools.ietf.org/html/draft-ietf-6tisch-minimal-security-15)) over IEEE 802.15.4 radio and 6TiSCH, with OSCORE context establishment and routing tree formation with RPL, on OpenMote nodes.

We plan to use this base, purposely designed to be extensible above the libraries providing OSCORE and COSE support, to implement and integrate complementary upcoming secure IoT protocols such as EDHOC ([Ephemeral Diffie-Hellman Over COSE](https://tools.ietf.org/html/draft-ietf-lake-edhoc)) which build upon OSCORE and COSE.

Furthermore, we use this platform to benchmark and compare different secure IoT protocols stacks, for example as described in our [other publication on this topic](https://arxiv.org/pdf/2011.12035.pdf) comparatively evaluating DTLS1.3.


## Low-power Virtual Machines using rBPF

We designed rBPF, a register-based VM based on extended Berkeley Packet Filters (eBPF). In our [publication on this topic](https://arxiv.org/pdf/2011.12047.pdf), we show that rBPF execution time overhead is tolerable for low-throughput, low-energy IoT devices. We further show that, using a VM based on rBPF requires only negligible memory overhead (less than 10% more memory). Compared to an alternative such as WebAssembly for microcontrollers ([Wasm3](https://github.com/wasm3/wasm3)) which requires 100% overhead, rBPF is thus a promising approach to host small software modules, isolated from OS software, and updatable on-demand, over low-power networks.

This functionality is not yet available in the master branch of RIOT, but it is available in this fork.
To try out this functionality, start with the [gcoap_bpf example](examples/gcoap_bpf).


## Related Publications

- K. Zandberg et al. "[Secure firmware updates for constrained IoT devices using open standards: A reality check](https://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=8725488)." IEEE Access, 2019
- B. Moran et al. "[A Concise Binary Object Representation (CBOR)-based Serialization Format for the Software Updates for Internet of Things (SUIT) Manifest](https://tools.ietf.org/html/draft-ietf-suit-manifest-09)." IETF Internet Draft draft-ietf-suit-manifest-09, 2020
- K. Zandberg, E. Baccelli. "[Minimal Virtual Machines on IoT Microcontrollers: The Case of Berkeley Packet Filters with rBPF](https://arxiv.org/pdf/2011.12047.pdf)." IFIP/IEEE PEMWN, 2020
- G. Restuccia, et al. "[Low-Power IoT Communication Security: On the Performance of DTLS and TLS 1.3](https://arxiv.org/pdf/2011.12035.pdf)." IFIP/IEEE PEMWN, 2020
- T. Claeys et al. "[RIOT and OpenWSN 6TiSCH: Happy Together](https://hal.inria.fr/hal-03064601/document).", Inria Research Report, 2020
- RIOT [Release 2020.10](https://github.com/RIOT-OS/RIOT/releases/tag/2020.10)
- 6TiSCH [Tutorial](https://hal.inria.fr/hal-02420974/document)


## Required Tooling

- GCC, and the specific flavour required for the platform (follow [this guide](https://doc.riot-os.org/getting-started.html) to install the toolchain)
- [LLVM](https://llvm.org/docs/GettingStarted.html)/[Clang](https://clang.llvm.org/get_started.html)


[api-badge]: https://img.shields.io/badge/docs-API-informational.svg
[api-link]: https://riot-os.org/api/
[irc-badge]: https://img.shields.io/badge/chat-IRC-brightgreen.svg
[irc-link]: https://webchat.freenode.net?channels=%23riot-os
[license-badge]: https://img.shields.io/github/license/RIOT-OS/RIOT
[license-link]: https://github.com/RIOT-OS/RIOT/blob/master/LICENSE
[master-ci-badge]: https://ci.riot-os.org/RIOT-OS/RIOT/master/latest/badge.svg
[master-ci-link]: https://ci.riot-os.org/nightlies.html#master
[matrix-badge]: https://img.shields.io/badge/chat-Matrix-brightgreen.svg
[matrix-link]: https://matrix.to/#/#riot-os:matrix.org
[release-badge]: https://img.shields.io/github/release/RIOT-OS/RIOT.svg
[release-link]: https://github.com/RIOT-OS/RIOT/releases/latest
[stackoverflow-badge]: https://img.shields.io/badge/stackoverflow-%5Briot--os%5D-yellow
[stackoverflow-link]: https://stackoverflow.com/questions/tagged/riot-os
[twitter-badge]: https://img.shields.io/badge/social-Twitter-informational.svg
[twitter-link]: https://twitter.com/RIOT_OS
[wiki-badge]: https://img.shields.io/badge/docs-Wiki-informational.svg
[wiki-link]: https://github.com/RIOT-OS/RIOT/wiki
