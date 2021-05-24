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
#include <sys/types.h>
#include <cerrno>
#include <cctype>
#include <stack>
#include <algorithm>
#include <fstream>

using namespace std;

enum type_of_lex {
  LEX_NULL,                                                                                                                      /*00*/
  LEX_BOOL, LEX_BREAK, LEX_CONTINUE, LEX_DO, LEX_ELSE, LEX_FALSE, LEX_FOR, LEX_FUNCTION, LEX_GETENV, LEX_IF, LEX_IN,             /*11*/       
  LEX_NAN, LEX_NUMBER, LEX_NULLPTR, LEX_OBJECT, LEX_RETURN, LEX_STRING, LEX_TRUE, LEX_TYPEOF, 	                                 /*19*/
  LEX_UNDEFINED, LEX_VAR, LEX_WHILE, LEX_WRITE,                                                                                  /*23*/
  LEX_FIN,                                                                                                                       /*24*/
  LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_DOT, LEX_LPAREN, LEX_RPAREN, LEX_LQPAREN, LEX_RQPAREN, LEX_BEGIN, LEX_END,            /*34*/
  LEX_EQ, LEX_DEQ, LEX_TEQ, LEX_LSS, LEX_GTR, LEX_PLUS, LEX_PLUS_EQ, LEX_DPLUS, LEX_MINUS, LEX_MINUS_EQ, LEX_DMINUS,             /*45*/
  LEX_TIMES, LEX_TIMES_EQ, LEX_TIMES_SLASH, LEX_SLASH, LEX_SLASH_EQ, LEX_SLASH_TIMES, LEX_DSLASH, LEX_PERCENT, LEX_PERCENT_EQ,   /*54*/
  LEX_LEQ, LEX_NOT, LEX_NEQ, LEX_NDEQ, LEX_GEQ, LEX_OR, LEX_DPIPE, LEX_AND, LEX_DAMP,                                            /*63*/
  LEX_NUM,                                                                                                                       /*64*/
  LEX_STR_CONST,                                                                                                                 /*65*/                                                                       
  LEX_ID,                                                                                                                        /*66*/ 
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
    "getenv",
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
    "write",
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

vector<string> Table;

int FindVar(const string& buf){
    int k;
    bool flag = false;
    for (int i = 0; i < Table.size(); i++){
        if (Table[i] == buf){
            k = i;
            flag = true;
            break;
        }
    }

    if (flag==false){
        Table.push_back(buf);
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
    Lex(){ type = LEX_NULL; val = 0; str = "";}
    type_of_lex get_type() const { return type;}

    friend ostream& operator<<(ostream& out, const Lex& l){ 
        string t;
        if (l.type <= LEX_WRITE)
            t = TW[l.type];
        else if (l.type > LEX_FIN && l.type <= LEX_DAMP)
            t = TD[l.type - LEX_FIN];
        else switch(l.type) {
            case LEX_NUM:
                t = "NUMB";
                break;
            case LEX_STR_CONST:
                t = "string const";
                break;
            case LEX_ID:
                t = Table[l.val];
                break;
            default:
                cout << "problem on 190\n"; 
                throw l.type;
                break;
        }
        if (l.type == LEX_STR_CONST) {
            cout << "<" << t << " , " << l.str << ">" << '\n';
        }
        else {
            cout << "<" << t << " , " << l.val << ">" << '\n';
        }
        return out;
    }
};
 
enum state{ H, IDENT, DIDJ, COM1, COM2, COM3, SLASHS, EQ1, EQ2, PLUSS, MINUSS, AMP, PIPE, STR }; 

class Scanner {
    char ch;
    int info;
    bool flag = true;
    void gc() {  cin.read(&ch, 1);}
    int FindString(string str, const char** list){
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

    int setp(string& buf, state& st, char ch, int& digit) {
        if (!isspace(ch)) {
            if(cin.eof()) { return 1;}
            if (isalpha(ch)) {
                buf.push_back(ch);
                st = IDENT;
            } else if (isdigit(ch)) {
                digit = ch - '0';
                st = DIDJ;
            } else switch(ch) {
                case '/':
                    buf.push_back(ch);
                    st = SLASHS;
                    break;
                case '!': case '=':
                    buf.push_back(ch);
                    st = EQ1;
                    break;
                case '*': case '<': case '>': case '%':
                    buf.push_back(ch);
                    st = EQ2;
                    break;
                case '+':
                    buf.push_back(ch);
                    st = PLUSS;
                    break;
                case '-':
                    buf.push_back(ch);
                    st = MINUSS;
                    break;
                case '&':
                    buf.push_back(ch);
                    st = AMP;
                    break;
                case '|':
                    buf.push_back(ch);
                    st = PIPE;
                    break;
                case '#':
                    st = COM3;
                    break;
                case '"':
                    st = STR;
                    break;
                default:
                    return 2;
            }
        }
        return 0;
    }

    Lex get_lex() {
        string buf;
        int digit, t, ind;
        state CurState = H;
        while (1) {
            if (flag) gc(); else flag = true;
            switch (CurState) {
                case H:
                    ind = setp(buf, CurState, ch, digit);
                    if (ind == 2){
                        buf.push_back(ch);
                        if ((t = FindString(buf, TD))){
                            return Lex((type_of_lex)(LEX_FIN + t), t);
                        } else throw ch;
                    }
                    if (ind == 1) return Lex(LEX_FIN);
                    break;
                case IDENT:
                    if (isdigit(ch) || isalpha(ch)) { 
                        buf.push_back(ch);
                    } else {
                        flag = false;
                        if ((t = FindString(buf, TW)) != 0) {
                            return Lex((type_of_lex)t, t);
                        } else {
                            t = FindVar(buf);
                            return Lex(LEX_ID, t);
                        }
                    }
                    break;
                case DIDJ:
                    if (isdigit(ch)) { digit = digit * 10 + (ch - '0');}
                    else if (isalpha(ch)) { throw ch;}
                    else {
                        flag = false;
                        return Lex(LEX_NUM, digit);
                    }
                    break;
                case SLASHS:
                    if (ch == '*'){
                        buf.pop_back();
                        CurState = COM1;
                    }
                    else if (ch == '='){
                        buf.push_back(ch);
                        t = FindString(buf, TD);
                        return Lex((type_of_lex)(LEX_FIN + t), t);
                    }
                    else if (ch == '/'){
                        buf.pop_back();
                        CurState = COM3;
                    }
                    else {
                        flag = false;
                        t = FindString(buf, TD);
                        return Lex((type_of_lex)(LEX_FIN + t), t);
                    }
                    break;
                case COM1:
                    if (ch == '*'){
                        CurState = COM2;
                    } else if (cin.eof()) throw ch;
                    break;
                case COM2:
                    if (ch == '/') {
                        CurState = H;
                    } else if (cin.eof())
                        throw ch;
                    else
                        CurState = COM1;
                    break;
                case COM3:
                    if (ch == '\n')
                        CurState = H;
                    else if (cin.eof())
                        throw ch;
                    break;
                case EQ1:
                    if (ch == '='){
                        buf.push_back(ch);
                        CurState = EQ2;
                    } else {
                        flag = false;
                        t = FindString(buf, TD);
                        return Lex((type_of_lex)(LEX_FIN + t), t);
                    }
                    break;
                case EQ2:
                    if (ch == '=') {
                        buf.push_back(ch);
                        t = FindString(buf, TD);
                        return Lex((type_of_lex)(LEX_FIN + t), t);
                    } else {
                        flag = false;
                        t = FindString(buf, TD);
                        return Lex((type_of_lex)(LEX_FIN + t), t);
                    }
                    break;
                case PLUSS:
                    if (ch == '=' || ch == '+') {
                        buf.push_back(ch);
                        t = FindString(buf, TD);
                        return Lex((type_of_lex)(LEX_FIN + t), t);
                    } else {
                        flag = false;
                        t = FindString(buf, TD);
                        return Lex((type_of_lex)(LEX_FIN + t), t);
                    }
                    break;
                case MINUSS:
                    if (ch == '=' || ch == '-') {
                        buf.push_back(ch);
                        t = FindString(buf, TD);
                        return Lex((type_of_lex)(LEX_FIN + t), t);
                    } else {
                        flag = false;
                        t = FindString(buf, TD);
                        return Lex((type_of_lex)(LEX_FIN + t), t);
                    }
                    break;
                case AMP:
                    if (ch == '&') {
                        buf.push_back(ch);
                        t = FindString(buf, TD);
                        return Lex((type_of_lex)(LEX_FIN + t), t);
                    } else {
                        flag = false;
                        t = FindString(buf, TD);
                        return Lex((type_of_lex)(LEX_FIN + t), t);
                    }
                    break;
                case PIPE:
                    if (ch == '|') {
                        buf.push_back(ch);
                        t = FindString(buf, TD);
                        return Lex((type_of_lex)(LEX_FIN + t), t);
                    } else {
                        flag = false;
                        t = FindString(buf, TD);
                        return Lex((type_of_lex)(LEX_FIN + t), t);
                    }
                    break;
                case STR:
                    if (ch == '"') {
                        string res = "";
                        for (int i = 0; i < buf.size(); i++){
                            res += buf[i];
                        }
                        return Lex(LEX_STR_CONST, 0, res);
                    } else if (cin.eof())
                        throw ch;
                    buf.push_back(ch);
                    break;
            }
        }
    }
};


class Parser {
    Scanner scan;

    Lex curr_lex; // текущая лексема
    type_of_lex c_type; // ее тип 
    int c_val; // ее значение (чтобы часто не пользоваться геттерами)

    void gl(){
        curr_lex = scan.get_lex();
        c_type = curr_lex.get_type();
    }

    void B();
    void S();
    void IF();
    void WHILE();
    void DO();
    void FOR();
    void FOR_PARM();
    void ID();
    void BREAK();
    void WRITE();
    void E();
    void E1();
    void T();
    void F();
    void D();
public:
    Parser(): scan() {}
    Parser(const char* file) : scan(file) {}
    void analyze();

};

void Parser::analyze() {
    gl();
    S();
    if (c_type != LEX_FIN) throw Error("analiz's error");
    cout << "YSE\n";
}

void Parser::S() {
    switch (c_type){
        case LEX_IF:
            IF();
            break;
        case LEX_WHILE:
            WHILE();
            break;
        case LEX_DO:
            DO();
            break;
        case LEX_FOR:
            FOR();
            break;
        case LEX_ID:
            ID();
            break;
        case LEX_BREAK:
            BREAK();
            break;
        case LEX_VAR:
            gl();
            D(); 
            break;
        case LEX_NUM:
            E();
            gl();
            break;
        case LEX_WRITE:
            WRITE();
            gl();
            break;
        case LEX_DPLUS: case LEX_DMINUS:
            E();
            gl();
            break;
        case LEX_FIN: case LEX_END:
            return;
    }
    S();
}

void Parser::IF() {
    gl();
        if (c_type != LEX_LPAREN)
            throw curr_lex;
        else{
            gl();
            E();
            if (c_type == LEX_RPAREN){
                gl();
                B();
                if (c_type == LEX_ELSE){
                    gl();
                    B();
                }
            }
            else
                throw curr_lex;
        }
}

void Parser::WHILE() {
    gl();
    if (c_type != LEX_LPAREN){          
        throw curr_lex;
    }
    else
    {
        gl();
        E();
        if (c_type == LEX_RPAREN)
        {
            gl();
            B();
        }
        else{
            throw curr_lex;
        }
    }
}

void Parser::ID() {
    gl();
    if (c_type == LEX_EQ || c_type == LEX_MINUS_EQ || c_type == LEX_PLUS_EQ || c_type == LEX_TIMES_EQ || c_type == LEX_PERCENT_EQ || c_type == LEX_SLASH_EQ) {
        gl();
	    E();
        while (c_type == LEX_EQ){
            gl();
	        E();
        }
        if (c_type == LEX_SEMICOLON) gl();
        else if (c_type == LEX_FIN) return;
        else {throw curr_lex;}
    } else if (c_type == LEX_DPLUS || c_type == LEX_DMINUS) {
        gl();
        if (c_type != LEX_SEMICOLON && c_type != LEX_FIN) throw curr_lex;
        gl();
    }
    else throw curr_lex;
}


void Parser::DO() {
        gl();
        B();
        if (c_type == LEX_WHILE)
        {
            gl();
            if (c_type == LEX_LPAREN)
            {
                gl();
                E();
                if (c_type != LEX_RPAREN)
                    throw curr_lex;
                gl();
                gl();
            }
            else
                throw curr_lex;
        }
        else
            throw curr_lex;
}

void Parser::FOR(){
    gl();
    if (c_type == LEX_LPAREN){
        gl();
        FOR_PARM();
        if (c_type == LEX_SEMICOLON){
            gl();
            if (c_type != LEX_SEMICOLON){
                E();
            }
            if (c_type == LEX_SEMICOLON){
                gl();
                FOR_PARM();
                if (c_type == LEX_RPAREN){
                    gl();
                    B();
                }
                else
                    throw curr_lex;
            }
            else
                throw curr_lex;
        }
    }
    else
        throw curr_lex;
}


void Parser::FOR_PARM(){
    if (c_type != LEX_SEMICOLON && c_type != LEX_RPAREN){
        gl();
        if (c_type == LEX_EQ || c_type == LEX_MINUS_EQ || c_type == LEX_PLUS_EQ
            || c_type == LEX_TIMES_EQ || c_type == LEX_PERCENT_EQ || c_type == LEX_SLASH_EQ){
            gl();
            E();
        }
        else if (c_type == LEX_DPLUS)
        {
            gl();
            if (c_type != LEX_SEMICOLON && c_type != LEX_RPAREN)
                throw curr_lex;
            if (c_type == LEX_SEMICOLON)
                gl();
        }
        else if (c_type == LEX_DMINUS)
        {
            gl();
            if (c_type != LEX_SEMICOLON && c_type != LEX_RPAREN)
                throw curr_lex;
            if (c_type == LEX_SEMICOLON)
                gl();
        }
        else
            throw curr_lex;
    }
}


void Parser::BREAK() {
    gl();
    if (c_type != LEX_SEMICOLON && c_type != LEX_FIN) throw curr_lex;
    gl();
}

void Parser::WRITE() {
    gl();
    if (c_type != LEX_LPAREN) 
        throw curr_lex;
    gl();
    E();
    if (c_type != LEX_RPAREN) 
        throw curr_lex;
    gl();
    if (c_type != LEX_SEMICOLON) 
        throw curr_lex;
}


void Parser::B(){
    if (c_type == LEX_BEGIN){
        gl();
        S();
        if (c_type == LEX_END){
            gl();
        }
        else{
            throw curr_lex;
        }
    }
    else
        throw curr_lex;
}


void Parser::D(){
    if (c_type != LEX_ID)
        throw curr_lex;
    else{
        gl();
        if (c_type == LEX_EQ){
            gl();
            E();  
        }
        while (c_type == LEX_COMMA)
        {
            gl();
            if (c_type != LEX_ID)
                throw curr_lex;
            else{
                gl();
                if (c_type == LEX_EQ) {
                    gl();
                    E();
                }
            }
        }
        if (c_type != LEX_SEMICOLON && c_type != LEX_FIN)
            throw curr_lex;
        else if (c_type == LEX_SEMICOLON){
            gl();
        }
    }
}


void Parser::E(){
    if (c_type == LEX_DPLUS || c_type == LEX_DMINUS){
        gl();
        if (c_type != LEX_ID){
            throw curr_lex;
        }
        gl();
    }
    else{
        E1();
        if (c_type == LEX_EQ || c_type == LEX_LSS || c_type == LEX_DEQ || c_type == LEX_GTR || c_type == LEX_LEQ
            || c_type == LEX_TEQ || c_type == LEX_GEQ || c_type == LEX_NEQ || c_type == LEX_DMINUS
            || c_type == LEX_DPLUS){
            if (!(c_type == LEX_DMINUS || c_type == LEX_DPLUS)){
                gl();
                E1();
            }
            else{
                if (c_type == LEX_DMINUS || c_type == LEX_DPLUS){
                    gl();
                }
            }
        }
    }
}

void Parser::E1()
{
    T();
    while (c_type == LEX_PLUS || c_type == LEX_MINUS || c_type == LEX_DPIPE){
        gl();
        T();
    }
}

void Parser::T()
{
    F();
    while (c_type == LEX_TIMES || c_type == LEX_SLASH || c_type == LEX_DAMP || c_type == LEX_PERCENT){
        gl();
        F();
    }
}

void Parser::F() {
    switch (c_type){
    case LEX_ID:
        gl();
        break;
    case LEX_NUM:
        gl();
        break;
    case LEX_TRUE:
        gl();
        break;
    case LEX_FALSE:
        gl();
        break;
    case LEX_STR_CONST:
        gl();
        break;
    case LEX_NOT:
        gl();
        F();
        break;
    case LEX_LPAREN:
        gl();
        E();
        if (c_type == LEX_RPAREN) gl();
        else throw curr_lex;
    default:
        throw curr_lex;
    }   
}

int main(int argc, char** argv) {
    Parser p;
    Lex lexem;
    Scanner s;
    if (argc >= 2) p = Parser(argv[1]);
    else if (argc == 1) p = Parser();

    if (argc >= 2) s = Scanner(argv[1]);
    else if (argc == 1) s = Scanner();

    try{
        // do{
        //     lexem = s.get_lex();
        //     if (lexem.get_type() == LEX_FIN){
        //         break;
        //     }
        //     else{
        //         cout<< lexem;
        //     }
        // }while(true);
        p.analyze();
    }
    catch (Error &errr) {
            cout << errr;
        }
    catch (type_of_lex &a){
        cout << "NO\ntype\n" << a << '\n';
    }
    catch (Lex &l){
        cout << "NO\nlex\n" << l.get_type() << '\n';
    }
    return 0;
}