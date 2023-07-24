#include<arpa/inet.h>
#include<string>
#include<cstring>
#include<sys/socket.h>
#include<iostream>

using namespace  std;

class TcpSocket
{
public:
    TcpSocket();
    TcpSocket(int fd);
    TcpSocket(string msg);
    ~TcpSocket();
    int getfd(){return fd;}
    int getresvfd(){return recv_fd;}
    int SendMsg(string msg);
    string RecvMsg();

    int readn(char * buf,int size);
    int writen(const char *msg,int size);


private:
    int fd=-1;//向服务器发送消息
    int recv_fd=-1;//接收服务器发送的消息
};