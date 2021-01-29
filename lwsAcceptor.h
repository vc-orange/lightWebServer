#ifndef LWS_ACCEPTOR
#define LWS_ACCEPTOR

#include "lwsChannel.h"
#include "lwsSocket.h"
#include <sys/socket.h>
#include <netinet/in.h>
class lws_event_loop;

class lws_acceptor
{
public:
typedef std::function<void(int socket_fd,const sockaddr& addr)> new_connection_callback;
    lws_acceptor(lws_event_loop* loop,const sockaddr& listen_addr);
    ~lws_acceptor();

    void set_new_connection_callback(const new_connection_callback call_back);
    bool listenning();
    void listen();
private:
    void handle_read();

    lws_event_loop* __loop;
    lws_socket __accept_socket;
    lws_channel __accept_channel;

    new_connection_callback __new_connection_callback;
    bool __listening;
};





#endif