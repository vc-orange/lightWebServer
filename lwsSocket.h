#ifndef LWS_SOCKET
#define LWS_SOCKET

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
class lws_socket{
public:
lws_socket(int socket_fd);
~lws_socket();

int fd();

void bind_address(const sockaddr& addr);

void listen_fd();

int accept_fd(sockaddr& clinet_addr);

void set_reuse_addr(bool on);

bool acceptable();

static int create_socket_fd();

static void close_socket_fd(int fd);

static void set_nonBlock(int sockfd);
private:
int __socket_fd;

};


void make_server_addr(sockaddr* addr,uint16_t port);
void make_addr(sockaddr* addr,const std::string& ip,uint16_t port);




#endif