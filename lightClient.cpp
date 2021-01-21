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
#include <sys/select.h>


const int port = 8888;
#define RCV_BUFF_SIZE 1024
#define SND_BUFF_SIZE 520

int main(int argc, char *argv[])
{
    std::string name;
    std::cout<<"输入名称: ";
    std::cin>>name;
    std::cout<<"欢迎进入聊天室"<<std::endl;
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

    std::string sendMessage = name+" Connect to Server\n";
    ret = send(sock, sendMessage.data(), sizeof(sendMessage)+1, 0);
    assert(ret!=-1);

    int keyInput=open("/dev/tty",O_RDONLY|O_NONBLOCK);
    assert(keyInput>0);

    int max_fd=keyInput>sock?keyInput:sock;
    fd_set read_fd;
    char sendMsg[SND_BUFF_SIZE];
    strcpy(sendMsg,name.data());
    strcat(sendMsg," : ");
    int nameLenth=strlen(sendMsg);
    char recvMsg[RCV_BUFF_SIZE];
    char nameRcv[20];
    while(1){
        FD_ZERO(&read_fd);
        FD_SET(keyInput,&read_fd);
        FD_SET(sock,&read_fd);
        select(max_fd+1,&read_fd,NULL,NULL,NULL);
        if(FD_ISSET(keyInput,&read_fd)){
            memset(sendMsg+nameLenth,0,SND_BUFF_SIZE-nameLenth);
            ret = read(keyInput,sendMsg+nameLenth,SND_BUFF_SIZE);
            send(sock,sendMsg,strlen(sendMsg),0);
            printf("\r");
        }
        if(FD_ISSET(sock,&read_fd)){
            ret = recv(sock,&recvMsg,RCV_BUFF_SIZE,0);
            if(ret ==0 ){
                std::cout<<"服务端关闭"<<std::endl;
                break;
            }
            recvMsg[ret]='\0';
            if(strncmp(name.data(),recvMsg,name.size())!=0)
                std::cout<<recvMsg;
        }
    }
    close(sock);
    return 0;
}