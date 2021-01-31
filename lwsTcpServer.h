#ifndef LWS_TCP_SERVER
#define LWS_TCP_SERVER

#include <netinet/in.h>
#include <map>
#include <memory>
#include <functional>
#include "lwsTcpConnection.h"

class lws_event_loop;
class lws_tcp_connection;
class lws_acceptor;

typedef lws_tcp_connection::connection_callback connection_callback;
typedef lws_tcp_connection::message_callback message_callback;
class lws_tcp_server
{
public:
    lws_tcp_server(lws_event_loop *loop, const sockaddr &client_addr);
    ~lws_tcp_server();

    void start();

    void set_connection_callback(const connection_callback &call_back);

    void set_message_callback(const message_callback &call_back);

private:
    void new_connection(int sock_fd, const sockaddr &client_addr);

    lws_event_loop *__loop;
    const std::string __name;

    std::unique_ptr<lws_acceptor> __acceptor;
    connection_callback __connection_call_back;
    message_callback __message_call_back;

    bool __started;
    int __next_connetion_id;

    typedef std::shared_ptr<lws_tcp_connection> tcp_connection_ptr;
    typedef std::map<std::string, tcp_connection_ptr> connection_map;
    connection_map __connections;
};

#endif