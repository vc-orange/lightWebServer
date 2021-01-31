#include "lwsTcpConnection.h"
#include "lwsEventLoop.h"
#include "lwsSocket.h"
#include "lwsChannel.h"
#include "unistd.h"
lws_tcp_connection::lws_tcp_connection(lws_event_loop *loop,
                                           std::string name,
                                           int sock_fd,
                                           sockaddr local_addr,
                                           sockaddr client_addr)
    : __loop(loop),
    __name(name),
    __channel(new lws_channel(loop,sock_fd)),
    __state(CONNECTING),
    __local_addr(local_addr),
    __client_addr(client_addr)
{
}

void lws_tcp_connection::set_connection_callback(connection_callback call_back)
{
    __connection_call_back=call_back;
}

void lws_tcp_connection::set_message_callback(lws_tcp_connection::message_callback call_back)
{
    __message_call_back=call_back;
}


void lws_tcp_connection::handleRead()
{
    char buf[65536];
    ssize_t n= read(__channel->fd(),buf,sizeof(buf));
    __message_call_back(shared_from_this(),buf,n);
    
    if(n==0){
        //-----
    }
    
}

bool lws_tcp_connection::connected()
{
    return __state==CONNECTED;
}

const std::string& lws_tcp_connection::name()
{
    return __name;
}

const sockaddr& lws_tcp_connection::local_addr()
{
    return __local_addr;
}

const sockaddr& lws_tcp_connection::client_addr()
{
    return __client_addr;
}

lws_event_loop* lws_tcp_connection::get_loop() const{
    return __loop;
}

void lws_tcp_connection::set_state(state s)
{
    __state=s;
}

void lws_tcp_connection::connect_established()
{
    __loop->assert_in_loop_thread();
    assert(__state == CONNECTING);
    set_state(CONNECTED);
    __channel->set_read_call_back(std::bind(&lws_tcp_connection::handleRead,this));
    __channel->enable_reading();

    __connection_call_back(shared_from_this());
}