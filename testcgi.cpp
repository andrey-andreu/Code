#include <iostream> 
#include <stdlib.h>

using namespace std;

int main (){
    cout << "CONTENT_TYPE: " << getenv("CONTENT_TYPE") << "\n\n"; 
    cout << "<html><body>\n";
    cout << "Greetings! You put in the arguments::\n";
    cout << "QUERY_STRING: " << getenv("QUERY_STRING") << endl; 
    cout << "SERVER_ADDR: " << getenv("SERVER_ADDR") << endl;
    cout << "SERVER_PROTOCOL: " << getenv("SERVER_PROTOCOL") << endl;  
    cout << "SERVER_PORT: " << getenv("SERVER_PORT") << endl;
    cout << "SCRIPT_NAME: " << getenv("SCRIPT_NAME") << endl;   
    cout << "</body></html>\n";
    return 0; 
}