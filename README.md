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

- highlight our on-going contributions upstream to the RIOT master branch including, but not limited to, SUIT-compliant secure OS software update support, and support for secure 6TiSCH networking.
- offer a sneak peek at additional security mechanisms not (yet) integrated in RIOT master branch, and how this platform is used in the context of Sparta, e.g. a prototype of minimal virtual machines for software module hosting and isolation with rBPF.


## Prototype of SUIT-compliant secure IoT software updates

We co-author the SUIT standard proposed by IETF to secure IoT software updates. The [SUIT specifications](https://tools.ietf.org/html/draft-ietf-suit-manifest-09) specify a security architecture, and the necessary metadata and cryptography to secure software updates,
applicable on microcontroller-based devices, such as the ones RIOT runs on.
We integrate SUIT support as described in our [publication on this topic](https://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=8725488).
We support a workflow as depicted below.

To try this functionality, start with the [suit_update example](examples/suit_update).

<a href="https://github.com/future-proof-iot/H2020-Sparta-D6-2-Sparta-RIOT-fp">
  <img alt="SUIT" src="https://raw.githubusercontent.com/future-proof-iot/H2020-Sparta-D6-2-Sparta-RIOT-fp/deliverable-6-2/doc/figures/SUIT-update-workflow.jpg" width="600">
</a>


SUIT-update-workflow.svg

## Secure Low-power IoT Networking

We integrated Open-WSN, the standards-compliant open-source implementation of the 6TiSCH network stack, as described in our [publication on this topic](https://hal.inria.fr/hal-03064601/document).

To try out this functionality, start by trying out the steps described in the [dedicated documentation](http://doc.riot-os.org/group__pkg__openwsn.html).

We plan to use this base to implement and integrate support for upcoming secure IoT protocols such as EDHOC.

Furthermore, we use this platform to benchmark and compare different secure IoT protocols stacks, for example as decribed in our [other publication on this topic](https://arxiv.org/pdf/2011.12035.pdf) comparatively evaluating DTLS1.3.


## Prototype of Low-power Virtual Machines using rBPF

We designed rBPF, a register-based VM basedon extended Berkeley Packet Filters (eBPF). In our [publication on this topic](https://arxiv.org/pdf/2011.12047.pdf), we show that rBPF execution time overhead is tolerable for low-throughput, low-energy IoT devices. We further show that, using a VM based on rBPF requires only negligible memory overhead (less than 10% more memory). Compared to an alternative such as Wasm, rBPF is thus a promising approach to host small software modules, isolated from OS software, and updatable on-demand, over low-power networks.

To try out this functionality, start with the [gcoap_bpf example](examples/gcoap_bpf).


## Prerequisites / Required Tooling

- GCC, and the specific flavour required for the platform.
- LLVM/Clang



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
