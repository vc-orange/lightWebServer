#ifndef LWS_TCP_CONNECTION
#define LWS_TCP_CONNECTION

#include <memory>
#include <atomic>
#include <netinet/in.h>
#include <functional>

class lws_event_loop;
class lws_socket;
class lws_channel;

class lws_tcp_connection;

typedef std::shared_ptr<lws_tcp_connection> lws_tcp_connection_ptr;

class lws_tcp_connection : public::std::enable_shared_from_this<lws_tcp_connection>
{
public:
    lws_tcp_connection(lws_event_loop *loop, std::string name, int sock_fd,sockaddr local_addr,sockaddr client_addr);

    bool connected();
    lws_event_loop* get_loop()const;
    const std::string& name();
    const sockaddr& local_addr();
    const sockaddr& client_addr();

    typedef std::function<void(lws_tcp_connection_ptr connection_ptr)> connection_callback;
    typedef std::function<void(lws_tcp_connection_ptr connection_ptr, char buf[], ssize_t size)> message_callback;

    void set_connection_callback(connection_callback call_back);
    void set_message_callback(message_callback call_back);

    void connect_established();
private:
    enum state
    {
        CONNECTING,
        CONNECTED,
        DISCONNECTED
    };
    void set_state(state s);

    void handleRead();

    lws_event_loop *__loop;
    std::string __name;
    std::atomic<state> __state;

    std::shared_ptr<lws_socket> __socket;
    std::shared_ptr<lws_channel> __channel;

    sockaddr __local_addr;
    sockaddr __client_addr;

    connection_callback __connection_call_back;
    message_callback __message_call_back;
};

#endif