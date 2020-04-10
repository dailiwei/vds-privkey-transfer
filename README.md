vds-privkey-transfer v1.0
=====================================

vds-privkey-transfer
--------------------

Vds-privkey-transfer is a tool for converting VDS address and private key, the core of this tool is ECDSA signatures and secret/public key operations on curve secp256k1. 

Vds-privkey-transfer is applicable to all projects based on elliptic encryption curve, such as VDS、BTC、ETH.

Functions:
* Convert a vds private key to address.
* Create a vds private key and its wallet address.
* Create a vds masternode private key. 

Features:
* secp256k1 ECDSA signing/verification and key generation.
* Additive and multiplicative tweaking of secret/public keys.
* Serialization/parsing of secret keys, public keys, signatures.

For more information, see [vds-core](https://github.com/v-dimension/vds-core) or [libsecp256k1](https://github.com/bitcoin-core/secp256k1).

License
-------

Vds-privkey-transfer is released under the terms of the MIT license. See [COPYING](COPYING) for more
information or see https://opensource.org/licenses/MIT.

Build
-----

Vds-privkey-transfer is built using autotools:

    $ sudo bash install.sh

Usages
------

1.Convert a vds private key to address:

    $ vds-privkey-transfer getaddress "privkey key"

2.Create a vds private key and its wallet address:

    $ vds-privkey-transfer vaddress genkey

or

    $ vds-privkey-transfer genkey

3.Create a vds masternode private key:

    $ vds-privkey-transfer masternode genkey

Copyright
---------

This software is developed by [www.ivds.org](https://www.ivds.org).