
// enumerate the commands so we aren't dealing with bare integers
// NOTE: going to abreviate kernelsocketfun(ksf)

#define KSF_GENL_NAME "ksf"
#define KSF_GENL_VERSION 0x1

enum ksf_commands {
    KSF_CMD_UNSPEC,
    KSF_CMD_HELLO,

    __KSF_CMD_AFTER_LAST,
    KSF_CMD_MAX = __KSF_CMD_AFTER_LAST - 1
};

enum ksf_attrs {
    KSF_ATTR_UNSPEC,
    __KSF_ATTR_LAST,
    KSF_ATTR_MAX = __KSF_ATTR_LAST - 1
};
