SUMMARY = "My custom kernel driver"
DESCRIPTION = "Custom out-of-tree kernel module"
LICENSE = "GPL-2.0-only"

LIC_FILES_CHKSUM = "file://Readme;md5=20f65b970b6afd68144327f869d2a5d9"

SRC_URI = " \
    file://driver.c \
    file://Makefile \
    file://Readme \
"

S = "${UNPACKDIR}"

inherit module
