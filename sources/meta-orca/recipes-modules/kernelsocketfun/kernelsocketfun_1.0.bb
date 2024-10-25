DESCRIPTION="just a recipe for making generic netlink sockets in the kernel"

LICENSE="GPL-2.0-only"
LIC_FILES_CHKSUM="file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

inherit module

SRC_URI =   "file://kernelsocketfun.c \
            file://kernelsocketfun.h \
            file://Makefile \
            "

S="${WORKDIR}"

RPROVIDES:${PN} += "kernel-module-kernelsocketfun"

do_install:append() {
    install -m 0644 kernelsocketfun.h ${D}${includedir}/${PN}
}
