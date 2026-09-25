## learning embedded linux 

This is a custom Yocto / PetaLinux meta-layer used to store project-specific software applications, hardware drivers, and custom system configuration settings.


## compile kernel using sdk (toolchain)

1) extract sdk 
./sdk.sh -d ~/new_disk/abhi/project-myd/sdk-mydz070

2) source the environment script 

source /home/vvdn/new_disk/abhi/project-myd/sdk-mydz070/environment-setup-cortexa9t2hf-neon-xilinx-linux-gnueabi

# optional
note : note make sure to check bellow things after environment script
printenv CC    
printenv CROSS_COMPILE
printenv ARCH
which ${CROSS_COMPILE}gcc
${CROSS_COMPILE}gcc --version
printenv SDKTARGETSYSROOT   

expected output :
=> CC=arm-xilinx-linux-gnueabi-gcc
=> arm-xilinx-linux-gnueabi-
=>/home/vvdn/new_disk/abhi/project-myd/sdk-mydz070/sysroots/x86_64-petalinux-linux/usr/bin/arm-xilinx-linux-gnueabi/arm-xilinx-linux-gnueabi-gcc
=>arm-xilinx-linux-gnueabi-gcc (GCC) 13.3.0
Copyright (C) 2023 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
=> /home/vvdn/new_disk/abhi/project-myd/sdk-mydz070/sysroots/cortexa9t2hf-neon-xilinx-linux-gnueabi

3) change the directory kernel-source

export ARCH=arm
export CROSS_COMPILE=arm-xilinx-linux-gnueabi

make olddefconfig    //configure default configuration 
make -j4 zImage      //build kernel image



## check content of image.ub
mkimage -l image.ub

## check kernel release using make
make kernelreleasels
