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

class ksf_helper
{
    int family_id;
    struct nl_sock* sock;
    bool init_failure = false;

public:
    ksf_helper()
    {
        std::cout << "allocating our netlink socket." << std::endl;
        sock = nl_socket_alloc(); // Allocate the socket
        if (!sock) {
            std::cerr << "failed to allocate netlink socket" << std::endl;
            init_failure = true;
            return;
        }

        std::cout << "connecting to generic netlink." << std::endl;
        if (genl_connect(sock) < 0)
        {
            std::cerr << "failed to connect to generic netlink"<< std::endl;
            init_failure = true;
            return;
        }

        std::cout << "resolving family name." << std::endl;
        family_id = genl_ctrl_resolve(sock, KSF_GENL_NAME);
        if (family_id < 0) {
            std::cerr << "Failed to resolve socket name ksf"<< std::endl;
            init_failure = true;
            return;
        }

        std::cout << "modifying nl_socket callbacks." << std::endl;
        nl_socket_modify_cb(sock, NL_CB_MSG_IN, NL_CB_CUSTOM, &recv_msg, &sock);
    }

    ~ksf_helper()
    {
        std::cout << "freeing resources" << std::endl;
        nl_socket_free(sock);
    }

    bool send_ksf_hello_cmd()
    {
        struct nl_msg *msg = nlmsg_alloc();
        std::cout << "populating our netlink message" << std::endl;
        genlmsg_put(msg, NL_AUTO_PORT, NL_AUTO_SEQ, family_id, 0, NLM_F_REQUEST, KSF_CMD_HELLO, KSF_GENL_VERSION);
        if (!msg){
            std::cerr << "failed to allocate msg header";
            return false;
        }

        std::cout << "sending the ksf_hello message over generic netlink." << std::endl;
        return nl_send_auto(sock, msg);
    }

    bool send_ksf_log_message_cmd(std::string logMessage)
    {
        struct nl_msg *msg = nlmsg_alloc();
        std::cout << "populating our netlink message" << std::endl;
        genlmsg_put(msg, NL_AUTO_PORT, NL_AUTO_SEQ, family_id, 0, NLM_F_REQUEST, KSF_LOG_MESSAGE, KSF_GENL_VERSION);
        if (!msg){
            std::cerr << "failed to allocate msg header";
            return false;
        }

        // Below is an easy macro that will require the definition of the GOTO nla_put_failure
        NLA_PUT_STRING(msg, KSF_KERNEL_LOG_MESSAGE, logMessage.c_str());

        std::cout << "sending the ksf_log_message message over generic netlink." << std::endl;
        return nl_send_auto(sock, msg);

        nla_put_failure:
            std::cerr << "Failed to add log message to generic netlink message." << std::endl;
            nlmsg_free(msg);
            return false;
    }
};

void print_help_text()
{
    std::string helpText = " \
    binksf -- sends messages to the kernelsocketfun module via generic netlink.                                                             \n\
        -ksfHello: logs a preset message in the kernel logs, access via dmesg.                                                                  \n\
        -ksfLogMessage: \"[message]\" logs the given message in kernel logs, see dmesg. this requires at least one separate string to run.      \n\
        -help: displays this message.                                                                                                           \n\
    ";

    std::cout << helpText << std::endl;
}

int str_to_ksf_command(std::string cmdString)
{
    if ( (cmdString.compare("ksfHello") == 0) || (cmdString.compare("KSFHELLO") == 0) ||
        (cmdString.compare("ksfhello") == 0) || (cmdString.compare("ksf_hello") == 0) )
    {
        return KSF_CMD_HELLO;
    }
    else if ( (cmdString.compare("ksfLogMessage") == 0) || (cmdString.compare("KSFLOGMESSAGE") == 0) ||
                (cmdString.compare("ksflogmessage") == 0) || (cmdString.compare("ksf_log_message") == 0) )
    {
        return KSF_LOG_MESSAGE;
    }
    return KSF_CMD_UNSPEC;
}

int main(int argc, char* argv[]) {

    if (argc < 2)
    {
        print_help_text();
        return 1;
    }

    int ksfCmd = str_to_ksf_command(argv[1]);
    std::cout << "arg count is: " << argc << std::endl;
    
    std::cout << "constructing our ksf_helper" << std::endl;
    ksf_helper ksfHelper = ksf_helper();
    if (ksf_helper.init_failure == true)
    {
        return 1;
    }

    switch(ksfCmd)
    {
        case KSF_CMD_HELLO:
        {
            bool test = ksfHelper.send_ksf_hello_cmd();
            if (!test)
            {
                std::cerr << "failed to send the ksf hello command." << std::endl;
            }
            break;
        }
        case KSF_LOG_MESSAGE:
        {
            if (argc < 2)
            {
                print_help_text();
            }
            std::string logMessage = "";
            for (int i = 2; i < argc; i++)
            {
                logMessage.append(" ");
                logMessage.append(argv[i]);
            }
            bool test = ksfHelper.send_ksf_log_message_cmd(logMessage);
            if (!test)
            {
                std::cerr << "failed to send the ksf log message command." << std::endl;
            }
            break;
        }
        case KSF_CMD_UNSPEC:
        {
            print_help_text();
            break;
        }
        default:
        {
            print_help_text();
            break;
        }
    }

    return 0;
}