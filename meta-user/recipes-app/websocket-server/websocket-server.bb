SUMMARY = "Websocket Server Application for MYD-Y7Z020-V2"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

DEPENDS = "libxml2 libwebsockets"
PKG_CONFIG_DEPENDS = "libxml2"

inherit update-rc.d pkgconfig

INITSCRIPT_NAME = "ws_server"
INITSCRIPT_PARAMS = "defaults 99"

SRCREV = "8974e036914c6462104d2cfe654204411bcf6f85"
# SRC_URI[sha256sum] = "b2ce72810d7cb8d6b5deaf1005dab1cc56cc0b94fdc5be977479bfc391d0b89e"

# 1. Point to your zip archive (Yocto auto-detects .zip and unpacks it)
SRC_URI = "git://gitlab.vvdntech.com:8081/sgri_elps/sgri_elps.git;protocol=https;branch=simulator/websocket-api-tcu"

# 2. Tell Yocto that the source code lives inside the extracted folder name
S = "${WORKDIR}/git/Qa_software"

do_compile() {
    # This executes 'make' inside the ${S} (Qa_software) folder
    oe_runmake
}

do_install() {
    # 3. This triggers your Makefile's install target inside the folder
    oe_runmake DESTDIR=${D} install

    # 4. Install your init script from the extracted folder structure
    install -d ${D}${INIT_D_DIR}
    install -m 0755 ${S}/ws_server.init ${D}${INIT_D_DIR}/ws_server
}

FILES:${PN} += "${INIT_D_DIR}/ws_server"
