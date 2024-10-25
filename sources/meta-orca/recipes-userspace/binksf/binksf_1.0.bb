DESCRIPTION="just a userspace application for creating a netlink socket to talk to the corresponding kernel module"

LICENSE="GPL-2.0-only"
LIC_FILES_CHKSUM="file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

S="${WORKDIR}"
B="${WORKDIR}/build"

SRC_URI =   "file://main.cpp \
            file://CMakeLists.txt \
            file://Modules/FindLibnl.cmake \
            "

RDEPENDS:${PN} += "kernel-module-kernelsocketfun libnl "
DEPENDS = "kernelsocketfun libnl "

inherit cmake

do_install() {
    install -d ${D}${bindir}
    install -m 0755 binksf ${D}${bindir}
}
