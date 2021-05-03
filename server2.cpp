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
#include <sys/wait.h>

using namespace std;

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
        Err(const string er){where_is_error=er;}
        void print_error() const{
            cerr << where_is_error << '\n' << errno << '\n';
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
        if (send(sd, bytes.data(), bytes.size(), 0) == -1){
            throw Err("Write2's error");
        }
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
        string way, method, http;
        vector<string> name;
        vector<string> value;
    public:
        HttpRequest(vector<string>& s){
            int pos = s[0].find(" ");
            method = s[0].substr(0, pos);
            s[0].erase(0, pos+1);
            pos = s[0].find(" ");
            way = s[0].substr(1, pos-1);
            http = s[0].substr(pos+1);
            for (int i = 1; i < s.size()-1; i++){
                HttpHeader head(s[i]);
                value.push_back(head.get_value());
                name.push_back(head.get_name());
            }
        }

        string get_http() const {return http;} 
        string get_way() const {return way;}
        string get_method() const {return method;}
        vector<string> get_name() const {return name;}
        vector<string> get_value() const {return value;}
        
};

class HttpResponse{
    string protocol, code, data, body;
    vector<uint8_t> file;
    size_t size;
    public:
        HttpResponse(const HttpRequest& req){
            data = Time();
            vector<string> n = req.get_name();
            vector<string> v = req.get_value();
            char c;
            int fd;
            bool CGI;

            if (req.get_method() == "GET"){
                if (req.get_way().find("?") == std::string::npos)
                    CGI = false;
                else
                    CGI = true;


                if (CGI){

                    string way = req.get_way();
                    string CON = "CONTENT_TYPE=text/plain";
                    string QUER = "QUERY_STRING=";
                    string SADDR = "SERVER_ADDR=127.0.0.1";
                    string SPORT = "SERVER_PROTOCOL=";
                    string PORT = "SERVER_PORT=1234";
                    string SCRNAME = "SCRIPT_NAME=";
                    string quer, sname;
                    int pos1 = way.find("?");
                    sname = way.substr(0, pos1);
                    quer = way;
                    char** env = new char*[7];

                    env[0] = new char[CON.size()];
                    env[0] = (char *) CON.c_str();

                    env[1] = new char[QUER.size() + quer.size()];
                    string q1 = QUER + quer;
                    env[1] = (char *) q1.c_str();

                    cout << SADDR.size() << endl;
                    env[2] = new char[SADDR.size()];
                    env[2] = (char *) SADDR.c_str();

                    env[3] = new char[SPORT.size()];
                    string  sp = SPORT + req.get_http();
                    env[3] = (char *) sp.c_str();

                    env[4] = new char[PORT.size()];
                    env[4] = (char *) PORT.c_str();

                    env[5] = new char[SCRNAME.size() + sname.size()];
                    string sc = SCRNAME + sname;
                    env[5] = (char *) sc.c_str();
                    env[6] = NULL;

                    pid_t pid;
                    int status;

                    if ((pid = fork()) < 0) throw Err("Fork's error");
                    else if (pid == 0){
                        char** argv = new char*[2];
                        argv[0] = new char[sname.size()];
                        argv[0] = (char *) sname.c_str();
                        argv[1] = NULL;
                        cout << argv[0] << '\n';

                        fd = open("temporary.txt", O_CREAT | O_TRUNC | O_WRONLY, 0644);
                        if (fd == -1) throw Err("Temporary file's error");
                        dup2(fd, 1);
                        close(fd);
                        execve(argv[0], argv, env);
                        cout << "The program failed" << '\n';
                        exit(6);
                    }
                    else{
                        wait(&status);
                        fd = open("temporary.txt", O_RDONLY);
                        //if (fd == -1) throw Err("Temporary file's error 2");
                        int i = 0;
                        while(read(fd, &c, 1)) {
                            file.push_back(c);
                            cout << file[i];
                            i++;
                        }

                        if (WIFEXITED(status) && (WEXITSTATUS(status) != 6))
                            code = "HTTP/1.1 200 OK\0";
                        else
                            code = "HTTP/1.1 404 Not Found\0";
                    }
                }
                else{
                    fd = open(req.get_way().c_str(), O_RDONLY);
                    if (fd == -1){
                        code = "HTTP/1.1 404 Not Found\0";
                        close(fd);
                        fd = open("404.png", O_RDONLY);
                        if (fd == -1){
                            throw Err("Papich's error");
                        }
                    } 
                    else{
                        code = "HTTP/1.1 200 OK\0";
                    }
                    while(read(fd, &c, 1)) {
                        file.push_back(c);
                    }
                    
                }
                close(fd);
                body = "\r\nVersion: HTTP/1.1\r\nContent-length: " + to_string(file.size()) + "\r\n\r\n";
                
            }

        }
        vector<uint8_t> get_file() const {
            return file;
        }

        string get_responce() const {
                return body;
            }

        string get_code(){
            return code;
        }

        int get_size() const {return size;}

};

vector<string> SplitLines(string& s) {
    vector<string> lines;
    int pos = 0;
    string line;
    int i = 0;
    while ((pos = s.find("\r\n")) != std::string::npos) {
        if (pos!=0){
            line = s.substr(0, pos+1);
            lines.push_back(line);
        }
        s.erase(0, pos + 1);
    }
    lines.push_back(s);
    return lines;
}


void ProcessConnection(int cd, const SocketAddress& clAddr) {
    ConnectedSocket cs(cd);
    string request;
    cs.Read(request);
    int s = request.size();
    vector<string> lines = SplitLines(request);
    HttpRequest req(lines);
    HttpResponse res(req);
    cout << res.get_responce() << '\n';
    cs.Write(res.get_code());
    cs.Write(res.get_responce());
    cs.Write(res.get_file());
    cs.Shutdown();
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
