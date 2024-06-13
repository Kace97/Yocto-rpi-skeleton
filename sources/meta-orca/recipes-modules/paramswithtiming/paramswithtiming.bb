DESCRIPTION="Just a recipe to print something on a timed loop"

LICENSE="GPL-2.0-only"
LIC_FILES_CHKSUM="file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

inherit module

SRC_URI=    "file://paramswithtiming.c \
            file://Makefile \
            "

S="${WORKDIR}"

RPROVIDES:${PN} += "kernel-module-paramsWithTiming"
