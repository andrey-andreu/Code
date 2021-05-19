#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>
#include <iterator>
#include <sys/types.h>
#include <sys/wait.h>
#include <cerrno>
#include <cctype>
#include <stack>
#include <algorithm>
#include <fstream>

using namespace std;

class Error{
    string err;
    public:
        Error(const string e){err=e;}
        Error();
        friend ostream& operator<<(ostream& out, Error &e){
            cout << e.err << '\n';
            return out;
        }
};

enum type_of_lex {
    LNULL, // 01
    BOOL, BREAK, CONTINUE, DO, ELSE, FALSE, FOR, FUNCTION, GETENV, IF, IN, // 12   
    NAN, NUMBER, NULLPTR, OBJECT, RETURN, STRING, TRUE, TYPEOF, // 20
    UNDEFINED, VAR, WHILE, WRITE, INT, // 25
    FIN, // 26
    SEMICOLON, COMMA, COLON, DOT, LPAREN, RPAREN, LQPAREN, RQPAREN, BEGIN, END, // 35
    EQ, DEQ, TEQ, LSS, GTR, PLUS, PLUSEQ, DPLUS, MINUS, MINUS_EQ, DMINUS, // 45
    TIMES, TIMES_EQ, TIMES_SLASH, SLASH, SLASH_EQ, SLASH_TIMES, DSLASH, PERCENT, PERCENT_EQ, // 55
    LEQ, NOT, NEQ, NDEQ, GEQ, OR, DPIPE, AND, DAMP, // 64
    NUMB, // 65
    STR_CONST, // 66
    ID, // 67
    PLABEL, PADDRESS, PGO, PFGO, PLEFTINC, PRIGHTINC, PLEFTDEC, PRIGHTDEC, PWRITE, PGETENV // 77                                                    
};

const char* TW[] = 
{ 
    "", 
    "Boolean", 
    "break", 
    "continue", 
    "do", 
    "else", 
    "false", 
    "for", 
    "function", 
    "if", 
    "in", 
    "NaN", 
    "Number", 
    "null", 
    "Object", 
    "return", 
    "String", 
    "true", 
    "typeof", 
    "undefined", 
    "var", 
    "while", 
    NULL 
};

const char* TD[] = 
{ 
    "", 
    ";", 
    ",", 
    ":", 
    ".", 
    "(", 
    ")", 
    "[", 
    "]", 
    "{", 
    "}", 
    "=", 
    "==", 
    "===", 
    "<", 
    ">", 
    "+", 
    "+=", 
    "++", 
    "-", 
    "-=", 
    "--", 
    "*", 
    "*=", 
    "*/", 
    "/", 
    "/=", 
    "/*", 
    "//", 
    "%", 
    "%=", 
    "<=", 
    "!", 
    "!=", 
    "!==", 
    ">=", 
    "|", 
    "||", 
    "&", 
    "&&", 
    NULL 
};

vector<string> Table;

int FindConst(const string& buf){
    int k;
    bool flag = false;
    for (int i = 0; i < Table.size(); i++){
        if (Table[i] == buf){
            k = i;
            flag = true;
            break;
        }
    }

    if (!flag){
        Table.push_back(buf.c_str());
        k = Table.size() - 1;
    }
    return k;
}

class Lex {
    type_of_lex type;
    int val;
    string str, tem;
public:
    Lex(type_of_lex t, int v = 0, string s = "") {type = t, val = v, str = s;}
    Lex(){ type = LNULL; val = 0; str = "";}
    type_of_lex get_type() const { return type;}

    friend ostream& operator<<(ostream& out, Lex &l){
        if (l.type <= WHILE)
            l.tem = TW[l.type];
        else if (l.type > FIN && l.type <= DAMP)
            l.tem = TD[l.type - FIN];
        else switch(l.type) {
            case NUMB:
                l.tem = "NUMB";
                break;
            case STR_CONST:
                l.tem = "string const";
                break;
            case ID:
                l.tem = Table[l.val];
                break;
            case PLABEL:
                l.tem = "Label";
                break;
            case PADDRESS:
                l.tem = "Addr";
                break;
            case PGO:
                l.tem = "!";
                break;
            case PFGO:
                l.tem = "!F";
                break;
            case PLEFTINC:
                l.tem = "+#";
                break;
            case PRIGHTINC:
                l.tem = "#+";
                break;
            case PLEFTDEC:
                l.tem = "-#";
                break;
            case PRIGHTDEC:
                l.tem = "#-";
                break;
            default:
                throw Error ("lex's error");
                break;
        }
        if (l.type == STR_CONST) {
            cout << '\n' << "<" << l.tem << " , " << l.str << ">" << '\n';
        }
        else {
            cout << '\n' << "<" << l.tem << " , " << l.val << ">" << '\n';
        }
        return out;
    }

};

enum state{ H, IDENT, DIDJ, COM1, COM2, COM3, SLASHS, EQ1, EQ2, PLUSS, MINUSS, AMP, PIPE, STR };

class Scanner{
    char c;
    bool flag = true;
    int info;

    void gc() { cin.read(&c, 1);}
    int Search(string str, const char** list){
        int i = 0;
        while (list[i]) {
            if (str == list[i]) return i;
            i++;
        }
        return 0;
    }
    public:
        Scanner() = default;
        Scanner(const char* file){
            int fd = open(file, O_RDWR);
            if (fd == -1){
                throw Error("File don't open");
            }
            dup2(fd, 0);
            close(fd);
        }

        void step(string& buf, state& st, char c, int& digit) {
            if (!isspace(c)) {
                if(cin.eof()) { info = 1;}
                if (isalpha(c)) {
                    buf.push_back(c);
                    st = IDENT;
                } else if (isdigit(c)) {
                    digit = c - '0';
                    st = DIDJ;
                } else switch(c) {
                    case '-':
                        buf.push_back(c);
                        st = MINUSS;
                        break;
                    case '&':
                        buf.push_back(c);
                        st = AMP;
                        break;
                    case '+':
                        buf.push_back(c);
                        st = PLUSS;
                        break;
                    case '!': case '=':
                        buf.push_back(c);
                        st = EQ1;
                        break;
                    case '*': case '<': case '>': case '%':
                        buf.push_back(c);
                        st = EQ2;
                        break;
                    case '/':
                        buf.push_back(c);
                        st = SLASHS;
                        break;
                    case '#':
                        st = COM3;
                        break;
                    case '"':
                        st = STR;
                        break;
                    case '|':
                        buf.push_back(c);
                        st = PIPE;
                        break;
                    default:
                        info = 2;
                }
            }
            info = 0;
        } 


        Lex GetLex() {
            string buf;
            int digit, t;
            state CurState = H;
            do {
                if (flag) gc(); else flag = true;
                switch (CurState) {
                    case H:
                        step(buf, CurState, c, digit);
                        if (info == 2){
                            buf.push_back(c);
                            if ((t = Search(buf, TD))){
                                return Lex((type_of_lex)(FIN + t), t);
                            } else throw Error("error char c");
                        }
                        if (info == 1) return Lex(FIN);
                        break;
                    case IDENT:
                        if (isdigit(c) || isalpha(c)) { 
                            buf.push_back(c);
                        } else {
                            flag = false;
                            if ((t = Search(buf, TW)) != 0) {
                                return Lex((type_of_lex)t, t);
                            } else {
                                t = FindConst(buf);
                                return Lex(ID, t);
                            }
                        }
                        break;
                    case DIDJ:
                        if (isdigit(c)) { digit = digit * 10 + (c - '0');}
                        else if (isalpha(c)) { throw Error("error char c");}
                        else {
                            flag = false;
                            return Lex(NUMB, digit);
                        }
                        break;
                    case SLASH:
                        if (c == '*'){
                            buf.pop_back();
                            CurState = COM1;
                        }
                        else if (c == '='){
                            buf.push_back(c);
                            t = Search(buf, TD);
                            return Lex((type_of_lex)(FIN + t), t);
                        }
                        else if (c == '/'){
                            buf.pop_back();
                            CurState = COM3;
                        }
                        else {
                            flag = false;
                            t = Search(buf, TD);
                            return Lex((type_of_lex)(FIN + t), t);
                        }
                        break;
                    case COM1:
                        if (c == '*'){
                            CurState = COM2;
                        } else if (cin.eof()) throw Error("error char c");
                        break;
                    case COM2:
                        if (c == '/') {
                            CurState = H;
                        } else if (cin.eof())
                            throw Error("error char c");
                        else
                            CurState = COM1;
                        break;
                    case COM3:
                        if (c == '\n')
                            CurState = H;
                        else if (cin.eof())
                            throw Error("error char c");
                        break;
                    case EQ1:
                        if (c == '='){
                            buf.push_back(c);
                            CurState = EQ2;
                        } else {
                            flag = false;
                            t = Search(buf, TD);
                            return Lex((type_of_lex)(FIN + t), t);
                        }
                        break;
                    case EQ2:
                        if (c == '=') {
                            buf.push_back(c);
                            t = Search(buf, TD);
                            return Lex((type_of_lex)(FIN + t), t);
                        } else {
                            flag = false;
                            t = Search(buf, TD);
                            return Lex((type_of_lex)(FIN + t), t);
                        }
                        break;
                    case PLUS:
                        if (c == '=' || c == '+') {
                            buf.push_back(c);
                            t = Search(buf, TD);
                            return Lex((type_of_lex)(FIN + t), t);
                        } else {
                            flag = false;
                            t = Search(buf, TD);
                            return Lex((type_of_lex)(FIN + t), t);
                        }
                        break;
                    case MINUS:
                        if (c == '=' || c == '-') {
                            buf.push_back(c);
                            t = Search(buf, TD);
                            return Lex((type_of_lex)(FIN + t), t);
                        } else {
                            flag = false;
                            t = Search(buf, TD);
                            return Lex((type_of_lex)(FIN + t), t);
                        }
                        break;
                    case AMP:
                        if (c == '&') {
                            buf.push_back(c);
                            t = Search(buf, TD);
                            return Lex((type_of_lex)(FIN + t), t);
                        } else {
                            flag = false;
                            t = Search(buf, TD);
                            return Lex((type_of_lex)(FIN + t), t);
                        }
                        break;
                    case PIPE:
                        if (c == '|') {
                            buf.push_back(c);
                            t = Search(buf, TD);
                            return Lex((type_of_lex)(FIN + t), t);
                        } else {
                            flag = false;
                            t = Search(buf, TD);
                            return Lex((type_of_lex)(FIN + t), t);
                        }
                        break;
                    case STR:
                        if (c == '"') {
                            string res = "";
                            for (int i = 0; i < buf.size(); i++){
                                res += buf[i];
                            }
                            return Lex(STR_CONST, 0, res);
                        } else if (cin.eof())
                            throw Error("error char c");
                        buf.push_back(c);
                        break;
                }
            }while(true);
        }

};

int main(int argc, char** argv) {
    Scanner s;
    if (argc == 2) s = Scanner(argv[1]);
    else if (argc == 1) s = Scanner();

    Lex lexem;
    int number = 0;
    try{
        do{
            lexem = s.GetLex();
            if (lexem.get_type() == FIN){
                break;
            }
            else 
                cout << lexem;
        }while(true);
    }
    catch (Error &errr) {
            cout << errr;
        }
    return 0;
}