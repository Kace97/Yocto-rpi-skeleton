find_path(LIBNL_INCLUDE_DIRS netlink/netlink.h
    /usr/include
    /usr/include/libnl3
    /usr/local/include
    /usr/local/include/libnl3
)

find_library(libnl_LIBRARY NAMES nl nl-3)
find_library(libnl_GENL_LIBRARY NAMES nl-genl nl-genl-3)

if (libnl_INCLUED_DIR AND libnl_LIBRARY AND libnl_GENL_LIBRARY)
    set(libnl_FOUND TRUE)
endif (libnl_INCLUDE_DIR AND libnl_LIBRARY AND libnl_GENL_LIBRARY)

set(libnl_LIBRARIES ${libnl_LIBRARY} ${libnl_GENL_LIBRARY})
