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

using namespace std;
bool flag = false;
const string Time() {
    struct tm *ptr;
    time_t a;
    a = time(NULL);
    ptr = gmtime(&a);
    string t = asctime(ptr);
    t = t.substr(0, t.length() - 1);
    t += " GMT\n";
    t.insert(3, 1, ',');
    int pos = t.find("\n");
    t.erase(pos, 1);
    return t;
}

class Err{
        string where_is_error;
    public:
        Err() {}
        Err(string er){where_is_error=er;}
        void print_error() const{
            cerr << where_is_error << '\n';
        }
};


class SocketAddress {
  private:
    struct sockaddr_in saddr;
  public:
    SocketAddress(){
        saddr.sin_family = AF_INET;
    };
    SocketAddress(const char* addr, short port) {
        saddr.sin_family = AF_INET;
        saddr.sin_port = htons(port);
        saddr.sin_addr.s_addr = inet_addr(addr);
    };
    struct sockaddr* get_addr() const {
        return (sockaddr*) &saddr;
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
    void Write(const string& str) {
        if (send(sd, &str[0], str.size(), 0) == -1)
            throw Err("Write's error");
    };
    void Write(const vector<uint8_t>& bytes) {
        
    };
    void Read(string& str) {
        int buf_len = 1024;
        char buf[buf_len];
        if (recv(sd, buf, buf_len, 0) == -1)
            throw Err("Read's error");
        str = buf;
    };
    void Read (vector<uint8_t>& bytes) {
       
    };
};


class HttpHeader{
        string name;
        string value;
        public:
            HttpHeader(const string& n, const string& v) : name(n), value(v) {}
            HttpHeader(string line){
                int ind = line.find(':');
                name = line.substr(0, ind);
                line.erase(0, ind + 1);
                value = line;
            }
        string get_name() const {
            return name;
        }
        string get_value() const {
            return value;
        }
};

class HttpRequest{ 
        string referer, method, http;
        vector<string> name;
        vector<string> value;
        int size; 
    public:
        HttpRequest(vector<string>& s, int size_){
            size = size_;
            int pos = s[0].find(" ");
            method = s[0].substr(0, pos);
            s[0].erase(0, pos+1);
            pos = s[0].find(" ");
            referer = s[0].substr(0, pos);
            http = s[0].substr(pos+1);
            for (int i = 1; i < s.size()-1; i++){
                HttpHeader head(s[i]);
                value.push_back(head.get_value());
                name.push_back(head.get_name());
            }
        }

        const string get_http() const {return http;} 
        const string get_referer() const {return referer;}
        const string get_method() const {return method;}
        vector<string> get_name() const {return name;}
        vector<string> get_value() const {return value;}
        int get_size() const {return size;}

        void print_referer() const {
            cout << get_referer() << endl;
        }
        void print_http() const {
            cout << get_http() << endl;
        }
        
};

class HttpResponse{
    string protocol, code, data;
    string body;
    public:
        HttpResponse(HttpRequest& req){
            data = Time();
            vector<string> n = req.get_name();
            vector<string> v = req.get_value();
            body += req.get_http();
            if (req.get_method() == "GET"){
                int fd = open(req.get_referer().c_str(), O_RDONLY);
                if (fd == -1){
                    code = " 404 Not Found";
                    body += code;
                    body += "\n";
                    body += "Data: ";
                    body += data;
                    body += "\n";
                } 
                else{
                    code = " 200 Ok";
                    body += code;
                    body += "\n";
                    body += "Data: ";
                    body += data;
                    body += "\n";
                    for (int i=0; i < n.size(); i++){
                        body += n[i];
                        body += ": ";
                        body += v[i];
                        body += "\n";
                     }
                    body += "Content-Length: ";
                    body += to_string(req.get_size());
                    body += "\n";
                }
                close(fd);
            }
        }

        string get_responce() const {
                return body;
            }

        string get_code(){
            return code;
        }

        friend ostream &operator<<(ostream &out, HttpResponse &res){
            out << res.code << '\n';
            return out;
        }
};

vector<string> SplitLines(string& s) {
    vector<string> lines;
    int pos = 0;
    string line;
    while ((pos = s.find("\n")) != std::string::npos) {
        line = s.substr(0, pos);
        lines.push_back(line);
        s.erase(0, pos + 1);
    }
    lines.push_back(s);
    if (lines[lines.size()-2].empty()){
        flag = true;
    }
    return lines;
}

void ProcessConnection(int cd, const SocketAddress& clAddr) {
    ConnectedSocket cs(cd);
    string request;
    cs.Read(request);
    int s = request.size();
    vector<string> lines = SplitLines(request);
    HttpRequest req(lines, s);
    HttpResponse res(req);
    cs.Write(res.get_responce());
    //cs.Shutdown();
}

void ServerLoop() {
    SocketAddress serverAddr("127.0.0.1", 1234);
    ServerSocket ss;
    ss.Bind(serverAddr);
    ss.Listen(1);
    for(;;) {
        SocketAddress clAddr;
        int cd = ss.Accept(clAddr);
        ProcessConnection(cd, clAddr);
    }
}

int main(){
    try{
        ServerLoop();
    }
    catch (const Err &errror){errror.print_error();}
    return 0;
}
