DESCRIPTION="just a test recipe for making the most basic kernel module I could think of"

LICENSE="GPL-2.0-only"
LIC_FILES_CHKSUM="file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

inherit module

SRC_URI =   "file://helloworld.c \
            file://Makefile \
            "

S="${WORKDIR}"

RPROVIDES:${PN} += "kernel-module-helloworld"
