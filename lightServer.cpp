
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <sys/select.h>
#include <set>
#include <arpa/inet.h>

const int port = 8888;
#define RCV_BUFF_SIZE 1024
#define SND_BUFF_SIZE 520
#define LIMIT_FD_NUM 1024

enum clientCmd
{
    DEFAULT,
    CLOSE
};

int HandleClient(int fd_client, char *Msg)
{
    char request[RCV_BUFF_SIZE];

    int ret = recv(fd_client, request, RCV_BUFF_SIZE, 0);
    if (ret == 0)
    {
        return clientCmd::CLOSE;
    }
    if (ret != -1)
    {
        request[ret] = '\0';
        printf("%s\nsuccesseful message form clinet %d\n", request, fd_client);
    }
    strcat(Msg, request);
    return clientCmd::DEFAULT;
}

int main(int argc, char *argv[])
{

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = PF_INET;
    server_address.sin_addr.s_addr = htons(INADDR_ANY);
    server_address.sin_port = htons(port);

    int sock;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);

    int ret = bind(sock, (struct sockaddr *)&server_address, sizeof(server_address));
    assert(ret != -1);

    ret = listen(sock, 1);
    assert(ret != -1);

    std::set<int> rd_fds;
    rd_fds.insert(sock);
    std::vector<int> client_closed; //临时数组，记录要把哪些关闭的CLIENT_FD从RD_FDS上移除

    fd_set rd_fd_set;
    FD_ZERO(&rd_fd_set);
    FD_SET(sock, &rd_fd_set);

    struct sockaddr_in client;

    char recvMsg[RCV_BUFF_SIZE];
    socklen_t client_addrlength = sizeof(client);

    while (1)
    {
        select(*rd_fds.rbegin() + 1, &rd_fd_set, NULL, NULL, NULL);
        if (FD_ISSET(sock, &rd_fd_set))
        {

            int con_fd = accept(sock, (struct sockaddr *)&client, &client_addrlength);
            if (con_fd < 0)
            {
                printf("建立连接失败");
            }
            else
            {
                std::cout << "成功连接" << std::endl;
                rd_fds.insert(con_fd);
            }
        }

        for (auto i : rd_fds)
        {
            if (i == sock)
                continue;
            if (FD_ISSET(i, &rd_fd_set))
            {
                int cmd = HandleClient(i, recvMsg);
                if (cmd == -1)
                {
                    getpeername(i, (struct sockaddr *)&client, &client_addrlength);
                    std::cout << "发生错误，客户端FD为" << i << std::endl
                              << "对方IP为" << inet_ntoa(client.sin_addr) << std::endl;
                    client_closed.push_back(i);
                    continue;
                }
                switch (cmd)
                {
                case clientCmd::CLOSE:
                    getpeername(i, (struct sockaddr *)&client, &client_addrlength);
                    std::cout << "客户端:" << i << "断开连接" << std::endl;
                    client_closed.push_back(i);
                    close(i);
                    break;
                default:
                    break;
                }
            }
        }
        for (auto fd : client_closed)
        {
            rd_fds.erase(fd);
        }
        client_closed.clear();

        for (auto fd : rd_fds)
        {
            if (fd != sock)
            {
                send(fd, recvMsg, strlen(recvMsg), 0);
            }
        }
        memset(recvMsg, 0, RCV_BUFF_SIZE);
        FD_ZERO(&rd_fd_set);
        for (auto i : rd_fds)
        {
            FD_SET(i, &rd_fd_set);
        }
    }
    return 0;
}