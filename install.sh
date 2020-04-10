#!/bin/bash
set -e

# Check if user is root
if [ $(id -u) != "0" ]; then
    echo -e "\nYou must be 'root' user, please run 'sudo bash install.sh'\n"
    exit 1
fi

# Install depends
apt update
apt install unzip cmake make libtool-bin libssl-dev libboost-system-dev libboost-thread-dev libsodium-dev libsecp256k1-dev -y

# Build vds-privkey-transfer
mkdir build && cd build
cmake ..
make
make install

clear
echo -e "\n vds-privkey-transfer installed successfully ! \n"
exit
