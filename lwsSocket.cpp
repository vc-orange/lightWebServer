#include "lwsSocket.h"
#include "lwsLog.h"
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <string.h>
lws_socket::lws_socket(int socket_fd)
    : __socket_fd(socket_fd)
{
    assert(__socket_fd >= 0);
}
//close
lws_socket::~lws_socket()
{
    close_socket_fd(__socket_fd);
}

void lws_socket::close_socket_fd(int socket_fd)
{
    if (socket_fd > 0)
    {
        int ret=shutdown(socket_fd,SHUT_RDWR);
        assert(ret == 0);
        ret = close(socket_fd);
        assert(ret == 0);
    }
}
//socket
int lws_socket::create_socket_fd()
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        LOG_FATAL << "lws_socet::create_socket_fd 失败，创建socket返回值为-1";
    }
    return socket_fd;
}

int lws_socket::fd()
{
    return __socket_fd;
}
//bind
void lws_socket::bind_address(const sockaddr &addr)
{
    int ret = bind(__socket_fd, &addr, sizeof(addr));
    if (ret < 0)
    {
        LOG_FATAL << "lws_socket::bind_address 发生错误\n";
    }
}
//listen
void lws_socket::listen_fd()
{
    int ret = listen(__socket_fd, SOMAXCONN);
    if (ret == -1)
    {
        LOG_FATAL << "lws_socket::listen_fd 发生错误\n";
    }
}

bool lws_socket::acceptable(){
    fd_set fs;
    FD_ZERO(&fs);
    FD_SET(__socket_fd,&fs);
    timeval tm={0,0};
    if(select(__socket_fd+1,&fs,NULL,NULL,&tm))
        return true;
    return false;
}
//accept
int lws_socket::accept_fd(sockaddr &clinet_addr)
{
    socklen_t addrlen = sizeof(clinet_addr);
    int connfd = accept(__socket_fd,
                        &clinet_addr,
                        &addrlen);
    set_nonBlock(connfd);

    if (connfd < 0)
    {
        int tmp_errno = errno;
        LOG_ERROR << "lws_socket::accept_fd 出错，错误码为" << tmp_errno << "\n";
        switch (tmp_errno)
        {
        case EAGAIN:
        case ECONNABORTED:
        case EINTR:
        case EPROTO: 
        case EPERM:
        case EMFILE:
            break;
        default:
            abort();
        }
    }
    return connfd;
}

void lws_socket::set_nonBlock(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    int ret = fcntl(fd, F_SETFL, flags);
    if (ret == -1)
    {
        LOG_FATAL << "lws_socket::set_nonBlock出错\n";
    }

    flags = fcntl(fd, F_GETFD, 0);
    flags |= FD_CLOEXEC;
    ret = fcntl(fd, F_SETFD, flags);
    if (ret == -1)
    {
        LOG_FATAL << "lws_socket::set_nonBlock出错\n";
    }
}

void lws_socket::set_reuse_addr(bool on)
{
    int optval = on ? 1 : 0;
    int ret = setsockopt(__socket_fd, SOL_SOCKET, SO_REUSEADDR,
                         &optval, sizeof optval);
    if (ret == -1)
    {
        LOG_FATAL << "lws_socket::set_reuse_addr出错\n";
    }
}



void make_server_addr(sockaddr* addr,uint16_t port){
    sockaddr_in* in=(sockaddr_in*)addr;
    in->sin_family=AF_INET;
    in->sin_addr.s_addr=htonl(INADDR_ANY);
    in->sin_port=htons(port);
}

void make_addr(sockaddr* addr,const std::string& ip,uint16_t port)
{
    sockaddr_in* in=(sockaddr_in*)addr;
    in->sin_family=AF_INET;
    in->sin_addr.s_addr=inet_addr(ip.c_str());
    in->sin_port=htons(port);
}

int get_port(const sockaddr *addr)
{
    sockaddr_in *addr_in = (sockaddr_in *)addr;
    return ntohs(addr_in->sin_port);
}


void get_ip(const sockaddr *addr, char c[])
{
    sockaddr_in *addr_in = (sockaddr_in *)addr;
    strcpy(c, inet_ntoa(addr_in->sin_addr));
}

sockaddr get_sock_addr(int sock_fd)
{
    sockaddr addr;
    socklen_t addr_len=sizeof(addr);
    memset(&addr,0,sizeof(addr));
    int ret=getsockname(sock_fd,&addr,&addr_len);
    if(ret<0)
    {
        LOG_ERROR<<"get_sock_addr()\n";
        abort();
    }
    return addr;
}