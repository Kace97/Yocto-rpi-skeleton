#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <unistd.h>

#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>

#include <kernelsocketfun/kernelsocketfun.h>

static int recv_msg(struct nl_msg *msg, void* ctx)
{
    struct nlmsghdr *nlh = nlmsg_hdr(msg);
    struct genlmsghdr *gnlh = genlmsg_hdr(nlh);

    char* message;

    if (gnlh->cmd == KSF_MESSAGE_GET)
    {
        std::cout << "hey this is from the one we care aboot" << std::endl;
        struct nlattr *attrs[KSF_ATTR_MAX + 1];

        // The NULL in the line below is for setting an attribute policy.
        // This, if we cared, would allow us to set min&max lengths on an attribute.
        if(genlmsg_parse(nlh, 0, attrs, KSF_ATTR_MAX, NULL))
        {
            std::cerr << "Something is wrong..." << std::endl;
            return NL_SKIP;
        }
        message = nla_get_string(attrs[KSF_SECRET_MESSAGE]);
        std::cout << "our secret message was: " << message << std::endl;
    }

    return NL_OK;
}

static int ack_msg_received(struct nl_msg *msg, void* ctx)
{
    std::cout << "Ack from KSF received." << std::endl;
    return NL_OK;
}

class ksf_helper
{
    int family_id;
    struct nl_sock* sock;
    bool _init_failure = false;

public:
    ksf_helper()
    {
        std::cout << "allocating our netlink socket." << std::endl;
        sock = nl_socket_alloc(); // Allocate the socket
        if (!sock) {
            std::cerr << "failed to allocate netlink socket" << std::endl;
            _init_failure = true;
            return;
        }

        std::cout << "connecting to generic netlink." << std::endl;
        if (genl_connect(sock) < 0)
        {
            std::cerr << "failed to connect to generic netlink"<< std::endl;
            _init_failure = true;
            return;
        }

        std::cout << "resolving family name." << std::endl;
        family_id = genl_ctrl_resolve(sock, KSF_GENL_NAME);
        if (family_id < 0) {
            std::cerr << "Failed to resolve socket name ksf"<< std::endl;
            _init_failure = true;
            return;
        }

        std::cout << "modifying nl_socket callbacks." << std::endl;
        nl_socket_modify_cb(sock, NL_CB_MSG_IN, NL_CB_CUSTOM, &recv_msg, &sock);
        nl_socket_modify_cb(sock, NL_CB_SEND_ACK, NL_CB_CUSTOM, &ack_msg_received, &sock);
    }

    ~ksf_helper()
    {
        std::cout << "freeing resources" << std::endl;
        nl_socket_free(sock);
    }

    bool init_failure() const { return _init_failure; }

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
            nlmsg_free(msg);
            return false;
        }

        // Below is an easy macro that will require the definition of the GOTO nla_put_failure
        NLA_PUT_STRING(msg, KSF_KERNEL_LOG_MESSAGE, logMessage.c_str());

        std::cout << "Sending the ksf_log_message message over generic netlink." << std::endl;
        return nl_send_auto(sock, msg);

        nla_put_failure:
            std::cerr << "Failed to add log message to generic netlink message." << std::endl;
            nlmsg_free(msg);
            return false;
    }

    bool send_ksf_get_secret_message(ksf_message_select messageId)
    {
        struct nl_msg *msg = nlmsg_alloc();
        std::cout << "Populating ksf_get_secret_message message." << std::endl;
        genlmsg_put(msg, NL_AUTO_PORT, NL_AUTO_SEQ, family_id, 0, NLM_F_REQUEST, KSF_GET_SECRET_MESSAGE, KSF_GENL_VERSION);
        if (!msg)
        {
            std::cerr << "Failed to allocate msg header." << std::endl;
            nlmsg_free(msg);
            return false;
        }

        NLA_PUT_U8(msg, KSF_MESSAGE_SELECT, messageId);

        std::cout << "Sending our message request down to the ksf module." << std::endl;
        return nl_send_auto(sock, msg);

        nla_put_failure:
            std::cerr << "Failed to add the message ID into our message to kernelsocketfun. message ID was: " << std::endl;
            nlmsg_free(msg);
            return false;
    }

    void receive_messages()
    {
        nl_recvmsgs_default(sock);
    }
};

void print_help_text()
{
    std::string helpText = " \
    binksf -- sends messages to the kernelsocketfun module via generic netlink.                                                             \n\
        -ksfHello: logs a preset message in the kernel logs, access via dmesg.                                                                  \n\
        -ksfLogMessage: \"[message]\" logs the given message in kernel logs, see dmesg. this requires at least one separate string to run.      \n\
        -ksfGetSecretMessage: \"[password]\" sends a request down to the kernelsocketfun module which will respond with the corresponding secret. \n\
        -help: displays this message.                                                                                                           \n\
    ";

    std::cout << helpText << std::endl;
}

ksf_commands str_to_ksf_command(std::string cmdString)
{
    if ( (cmdString.compare("ksfHello") == 0) || (cmdString.compare("KSFHELLO") == 0) ||
        (cmdString.compare("ksfhello") == 0) || (cmdString.compare("ksf_hello") == 0) ||
        (cmdString.compare("KSF_HELLO") == 0) )
    {
        return KSF_CMD_HELLO;
    }
    else if ( (cmdString.compare("ksfLogMessage") == 0) || (cmdString.compare("KSFLOGMESSAGE") == 0) ||
                (cmdString.compare("ksflogmessage") == 0) || (cmdString.compare("ksf_log_message") == 0) ||
                (cmdString.compare("KSF_LOG_MESSAGE") == 0) )
    {
        return KSF_LOG_MESSAGE;
    }
    else if ( (cmdString.compare("KSFGETSECRETMESSAGE") == 0) || (cmdString.compare("ksfGetSecretMessage") == 0) ||
            (cmdString.compare("ksfgetsecretmessage") == 0) || (cmdString.compare("ksf_get_secret_message") == 0) ||
            (cmdString.compare("KSF_GET_SECRET_MESSAGE") == 0) )
    {
        return KSF_GET_SECRET_MESSAGE;
    }
    return KSF_CMD_UNSPEC;
}

ksf_message_select password_to_message_id(std::string password)
{
    if ( (password.compare("computer") == 0) || (password.compare("Computer") == 0) || (password.compare("COMPUTER") == 0) )
    {
        return KSF_COMPUTER_SECRET;
    }
    else if ( (password.compare("car") == 0) || (password.compare("Car") == 0) || (password.compare("CAR") == 0) )
    {
        return KSF_CAR_SECRET;
    }

    return KSF_MESSAGE_UNDEF;
}

int main(int argc, char* argv[]) {

    if (argc < 2)
    {
        print_help_text();
        return 1;
    }

    ksf_commands ksfCmd = str_to_ksf_command(argv[1]);
    
    std::cout << "constructing our ksf_helper" << std::endl;
    ksf_helper ksfHelper = ksf_helper();
    if (ksfHelper.init_failure() == true)
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
                std::cerr << "Failed to send the ksf hello command." << std::endl;
            }
            break;
        }
        case KSF_LOG_MESSAGE:
        {
            if (argc <= 2)
            {
                print_help_text();
                return 1;
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
                std::cerr << "Failed to send the ksf log message command." << std::endl;
            }
            break;
        }
        case KSF_GET_SECRET_MESSAGE:
        {
            if (argc != 3)
            {
                print_help_text();
                return 1;
            }
            ksf_message_select message_id = password_to_message_id(argv[2]);
            std::cout << "Message ID is: " << message_id << std::endl;
            bool test = ksfHelper.send_ksf_get_secret_message(message_id);
            if(!test)
            {
                std::cerr << "Failed to send the ksf_get_secret_message command" << std::endl;
            }
            ksfHelper.receive_messages();
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