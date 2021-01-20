#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <iostream>
#include <string>
const int port = 8888;
#define RCV_BUFF_SIZE 1024
#define SND_BUFF_SIZE 520

int main(int argc, char *argv[])
{

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = PF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(port);

    int sock;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);

    int ret = connect(sock, (struct sockaddr *)&server_address, sizeof(server_address));
    if(ret==-1){
        printf("Can not connect to %s",inet_ntoa(server_address.sin_addr));
    }

    std::string sendMessage = "hello Server";
    ret = send(sock, sendMessage.data(), sizeof(sendMessage), 0);
    assert(ret!=-1);

    
    while(std::cin>>sendMessage){
        ret = send(sock, sendMessage.data(), sizeof(sendMessage), 0);
        assert(ret!=-1);
        if(sendMessage.compare("close")==0)
            break;
    }
    close(sock);
    return 0;
}