#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>

#include <kernelsocketfun/kernelsocketfun.h>

static int recv_msg(struct nl_msg *msg, void* ctx)
{
    return NL_OK;
}

static int send_hello(struct nl_sock *sock, int family_id){
    struct nl_msg *msg = nlmsg_alloc();
    genlmsg_put(msg, NL_AUTO_PORT, NL_AUTO_SEQ, family_id, 0, NLM_F_REQUEST, KSF_CMD_HELLO, KSF_GENL_VERSION);
    if (!msg){
        std::cerr << "failed to allocate msg header";
        return 5;
    }
    return nl_send_auto(sock, msg);
}

int main() {
    struct nl_sock *sock;
    sock = nl_socket_alloc(); // Allocate the socket
    if (!sock) {
        std::cerr << "failed to allocate netlink socket" << std::endl;
        return 1;
    }

    if (genl_connect(sock) < 0)
    {
        std::cerr << "failed to connect to generic netlink"<< std::endl;
        nl_socket_free(sock);
        return 2;
    }

    int family_id = genl_ctrl_resolve(sock, KSF_GENL_NAME);
    if (family_id < 0) {
        nl_socket_free(sock);
        std::cerr << "Failed to resolve socket name ksf"<< std::endl;
        return 3;
    }
    
    nl_socket_modify_cb(sock, NL_CB_MSG_IN, NL_CB_CUSTOM, &recv_msg, &sock);

    if(send_hello(sock, family_id) < 0) {
        nl_socket_free(sock);
        std::cerr << "Failed to send commands over netlink sock"<< std::endl;
        return 4;
    }
    
}