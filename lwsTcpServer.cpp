#include "lwsTcpServer.h"
#include "lwsAcceptor.h"
#include "lwsEventLoop.h"
lws_tcp_server::lws_tcp_server(lws_event_loop *loop, const sockaddr &client_addr)
    : __loop(loop),
      __name("tmp_server"),
      __acceptor(new lws_acceptor(loop, client_addr)),
      __started(false),
      __next_connetion_id(1)
{
    __acceptor->set_new_connection_callback(
        std::bind(&lws_tcp_server::new_connection, this, std::placeholders::_1, std::placeholders::_2));
}

lws_tcp_server::~lws_tcp_server()
{
}


void lws_tcp_server::start()
{
    if(!__started){
        __started=true;
    }

    if(!__acceptor->listenning())
    {
        __loop->run_in_loop(
            std::bind(&lws_acceptor::listen,&(*__acceptor))
        );
    }
}

void lws_tcp_server::new_connection(int sock_fd, const sockaddr &client_addr)
{
    __loop->assert_in_loop_thread();
    char buf[32];
    snprintf(buf,sizeof(buf),"#%d",__next_connetion_id);
    __next_connetion_id++;
    std::string conn_name = __name + buf;

    LOG_INFO << "TcpServer::newConnection [" << __name
           << "] - new connection [" << conn_name
           << "] from " << get_port(&client_addr)<<'\n';
    
    sockaddr local_addr=get_sock_addr(sock_fd);

    lws_tcp_connection_ptr conn(
        new lws_tcp_connection(__loop,conn_name,sock_fd,local_addr,client_addr)
    );
    __connections[conn_name]=conn;
    conn->set_connection_callback(__connection_call_back);
    conn->set_message_callback(__message_call_back);
    conn->connect_established();

}



void lws_tcp_server::set_message_callback(const message_callback &call_back)
{
    __message_call_back = call_back;
}

void lws_tcp_server::set_connection_callback(const connection_callback &call_back)
{
    __connection_call_back = call_back;
}
