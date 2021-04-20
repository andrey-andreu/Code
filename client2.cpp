#include <iostream>
#include <stdio.h>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <chrono>
#include <ctime> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>
#include <errno.h>

using namespace std;
bool flag  = false;

class Err{
        string where_is_error;
    public:
        Err() {}
        Err(string er){where_is_error=er;}
        void print_error() const{
            cerr << where_is_error << '\n';
            cerr << errno;
        }
};


class SocketAddress {
  private:
    struct sockaddr_in saddr;
  public:
    SocketAddress(){
        saddr.sin_family = AF_INET;
    };
    SocketAddress(int port) {
        saddr.sin_family = AF_INET;
        saddr.sin_port = htons(port);
        saddr.sin_addr.s_addr = INADDR_ANY;
    };
    SocketAddress(const char* addr, short port) {
        saddr.sin_family = AF_INET;
        saddr.sin_port = htons(port);
        saddr.sin_addr.s_addr = inet_addr(addr);
    };
    struct sockaddr* get_addr() const {
        return (struct sockaddr*) &saddr;
    }
    socklen_t get_size() const {
        return sizeof(saddr);
    }
};

class Socket{
  protected:
    int sd;
    explicit Socket(int cd) : sd(cd) {};
  public:
    Socket() {
        sd = socket(AF_INET, SOCK_STREAM, 0);
        if (sd == -1)
            throw Err("Socket's error");
    }
    void Shutdown() {
        if (shutdown(sd, SHUT_RDWR) == -1){
            throw Err("Sutdown's error");
        };
    }
    void close_socket() {close(sd);}
    ~Socket() {
        close(sd);
    }
};

class ServerSocket : public Socket {
  public:
    ServerSocket() : Socket() {}
    void Bind(const SocketAddress& ipaddr) {
        if(bind(sd, ipaddr.get_addr(), ipaddr.get_size()) == -1)
            throw Err("Bind's error");
    };
    int Accept(SocketAddress& clAddr) {
        socklen_t size = clAddr.get_size();
        int res = accept(sd, clAddr.get_addr(), (socklen_t *)&size);
        if (res == -1)
            throw Err("Accept's error");
        return res;
    };
    void Listen(int backlog) {
        if (listen(sd, backlog) == -1)
            throw Err("Listen's error");
    };
};

class ConnectedSocket : public Socket {
  public:
    ConnectedSocket() = default;
    explicit ConnectedSocket(int sd) : Socket(sd) {};
    int Write(const string& str) {
        int k;
        if ((k =send(sd, &str[0], str.size(), 0)) == -1)
            throw Err("Write's error");
        return k;
    };
    void Write(const vector<uint8_t>& bytes) {
        
    };
    void Read(string& str) {
        int buf_len = 10240;
        char buf[buf_len];
        if (recv(sd, buf, buf_len, 0) == -1)
            throw Err("Read's error");
        str = buf;
    };
    void Read (vector<uint8_t>& bytes) {
       
    };
};

class ClientSocket: public ConnectedSocket{
    public:
        ClientSocket() : ConnectedSocket() {}
        void Connect(const SocketAddress& saddr){
            if (connect(sd, saddr.get_addr(), saddr.get_size()) == -1){
                throw Err("Connect's error");
            }
        }
};


int main(){
    try{
        ClientSocket s;
        SocketAddress saddr("127.0.0.1",1234);
        s.Connect(saddr);
        string send;
        string ret;
        while (1){
            getline(cin, send, '_');  
            s.Write(send);
            s.Read(ret);
            cout << ret << '\n';
            send.clear();
            ret.clear();
        }
    }
    catch(const Err &err){
        err.print_error();
    }
}
