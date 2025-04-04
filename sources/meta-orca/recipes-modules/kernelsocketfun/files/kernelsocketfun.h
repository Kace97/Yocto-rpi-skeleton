
// enumerate the commands so we aren't dealing with bare integers
// NOTE: going to abreviate kernelsocketfun(ksf)

#define KSF_GENL_NAME "ksf"
#define KSF_GENL_VERSION 0x1
#define KSF_GENL_GROUP

enum ksf_commands {
    KSF_CMD_UNSPEC,
    KSF_CMD_HELLO,
    KSF_LOG_MESSAGE,
    KSF_GET_SECRET_MESSAGE,

    KSF_MESSAGE_GET,
    KSF_MESSAGE_LOGGED,

    __KSF_CMD_AFTER_LAST,
    KSF_CMD_MAX = __KSF_CMD_AFTER_LAST - 1
};

enum ksf_attrs {
    KSF_ATTR_UNSPEC,

    KSF_KERNEL_LOG_MESSAGE,

    KSF_MESSAGE_SELECT,
    KSF_SECRET_MESSAGE,

    __KSF_ATTR_LAST,
    KSF_ATTR_MAX = __KSF_ATTR_LAST - 1,
};

enum ksf_message_select {
    KSF_SECRET_UNSPEC,
    KSF_CAR_SECRET,
    KSF_COMPUTER_SECRET,
    KSF_MESSAGE_UNDEF,
};
