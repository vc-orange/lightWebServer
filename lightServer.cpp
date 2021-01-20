
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

const int port = 8888;
#define RCV_BUFF_SIZE 1024
#define SND_BUFF_SIZE 520

int HandleClient(int fd_client)
{
    char request[RCV_BUFF_SIZE];

    while(recv(fd_client, request, RCV_BUFF_SIZE, 0)){
        if(strcmp(request,"close")==0)
            break;
        request[strlen(request) + 1] = '\0';
        printf("%s\nsuccesseful\n", request);
    }
    return 1;
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
    while (1)
    {
        struct sockaddr_in client;
        socklen_t client_addrlength = sizeof(client);

        int connfd = accept(sock, (struct sockaddr *)&client, &client_addrlength);
        if (connfd < 0)
        {
            printf("建立连接失败");
        }
        else
        {
            HandleClient(connfd);
            close(connfd);
            std::cout<<"已断开连接"<<std::endl<<std::endl;
        }
    }
    return 0;
}