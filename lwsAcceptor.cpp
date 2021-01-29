#include "lwsAcceptor.h"
#include "lwsEventLoop.h"
lws_acceptor::lws_acceptor(lws_event_loop *loop, const sockaddr &listen_addr)
    : __loop(loop),
      __accept_socket(lws_socket::create_socket_fd()),
      __accept_channel(loop, __accept_socket.fd()),
      __listening(false)
{
    __accept_socket.set_reuse_addr(true);
    __accept_socket.bind_address(listen_addr);
    __accept_channel.set_read_call_back(std::bind(&lws_acceptor::handle_read, this));
}

lws_acceptor::~lws_acceptor()
{
}

void lws_acceptor::set_new_connection_callback(const lws_acceptor::new_connection_callback call_back)
{
    __new_connection_callback = call_back;
}

bool lws_acceptor::listenning()
{
    return __listening;
}

void lws_acceptor::listen()
{
    __loop->assert_in_loop_thread();
    __listening = true;
    __accept_socket.listen_fd();
    __accept_channel.enable_reading();
}

const int max_count_fd = 5;
void lws_acceptor::handle_read()
{
    __loop->assert_in_loop_thread();
    sockaddr client_addr;

    int connfd;
    connfd = __accept_socket.accept_fd(client_addr);
    if (connfd >= 0)
    {
        if (__new_connection_callback)
        {
            __new_connection_callback(connfd, client_addr);
        }
        else
        {
            lws_socket::close_socket_fd(connfd);
        }
    }
}
