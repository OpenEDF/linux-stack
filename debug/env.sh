#!/bin/bash

# environment for uboot arm i.mx6ull myir toolchain
echo "!!! arm toolchain add env !!!"
source /opt/myir-imx-fb/meta-toolchain/environment-setup-cortexa7hf-neon-poky-linux-gnueabi
arm-poky-linux-gnueabi-gcc -v
