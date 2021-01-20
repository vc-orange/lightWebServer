
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <unistd.h>
const int port = 8888;
#define RCV_BUFF_SIZE 1024
#define SND_BUFF_SIZE 520

int handleClinet(int fd_connect)
{
    char request[RCV_BUFF_SIZE];
    recv(fd_connect, request, RCV_BUFF_SIZE, 0);
    request[strlen(request) + 1] = '\0';
    printf("%s\nsuccesseful\n", request);

    char buf[SND_BUFF_SIZE] = "HTTP/1.1 200 ok\r\nconnection:close\r\n\r\n";
    int s = send(fd_connect, buf, strlen(buf), 0);

    int fd = open("html_doc/hellp.html", O_RDONLY);
    assert(fd!=-1);
    printf("\nFileFd:%d\n",fd);
    sendfile(fd_connect, fd, NULL, 2500);
    close(fd);
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
            printf("errno\n");
        }
        else
        {
            handleClinet(connfd);
            close(connfd);
        }
    }
    return 0;
}