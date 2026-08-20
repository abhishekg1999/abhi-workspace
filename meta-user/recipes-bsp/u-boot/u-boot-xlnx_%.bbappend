FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append = " file://platform-top.h file://bsp.cfg"
SRC_URI += "file://user_2026-06-23-09-39-00.cfg \
            file://user_2026-06-23-09-55-00.cfg \
            file://user_2026-06-23-10-24-00.cfg \
            "

