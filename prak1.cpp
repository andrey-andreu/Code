#include <iostream>
using namespace std;

class Notebook
{
    private:
        string* note;
        int count;
    public:
        Notebook() {note = new string[1]; note[0] = string(); count=1;}
        Notebook(string name_user, string note_user) {
            note = new string[3];
            note[0] = name_user;
            note[1] = note_user;
            note[2] = string();
            count = 2;
        }

        Notebook(const Notebook &N){
            note = new string[count+1];
            for (int i=0; i<=count; i++){
                note[i] = N.note[i];
            }
            count = N.count;
        }

        Notebook& add(const string s){
            string* note_t;
            note_t = new string [count+2];
            for (int i=0; i<count; i++){
                note_t[i] = note[i];
            };
            note_t[count] = s;
            note_t[count+1] = string();
            count++;
            delete[] note;
            note = note_t;
            return *this;
        }

        Notebook& del(const string s){
            bool f=false;
            int k=-1;
            for (int i=0; i<=count; i++){
                if (note[i]==s){
                    f = true;
                    k = i;
                }
            }
            if (f){
                string* note_t;
                note_t = new string [count];
                int j = 0;
                for (int i=0; i<(count-1); i++){
                    if (i!=k){
                        note_t[j] = note[i];
                        j++;
                    }
                }
                note_t[count-1] = string();
                delete[] note;
                note = note_t;
                count--;
            }
            return *this;
        }

        void print_name(const string n, ostream& out){
            int i = 0;
            if(count != 0){
                while(i<=count){
                    if (note[i]==n){
                        while(note[i].empty()!=true){
                            out << note[i] << endl;
                            i++;
                        }
                    }
                    i++;
                }
                out << endl;
            }
        }


        friend Notebook operator+(const Notebook &s1, const Notebook &s2);
        friend ostream& operator<<(ostream& k, const Notebook& s);
        Notebook& operator=(Notebook N){
            if(count != 1) delete[] note;
            count = N.count;
            note = new string[count+1];
            for(int i = 0; i <= count; i++){
                note[i] = N.note[i];
            }
            return *this;
        }

        ~Notebook(){
            delete[] note;
            note = NULL;
        }
};

Notebook operator+(const Notebook &N1, const Notebook &N2){
    Notebook N;
    string* note_t;
    if ((N1.count!=1) || (N2.count!=1)){
        note_t = new string [N1.count+N2.count+2];
        for (int i=0; i <= N1.count; i++){
            note_t[i] = N1.note[i];
        };
        for (int i=0; i <= N2.count; i++){
            note_t[i+N1.count+1] = N2.note[i];
        };
        N.note = note_t;
        N.count = N1.count+N2.count+1;
    }
    return N;
}

ostream& operator<<(ostream& out, const Notebook &N){
    if(N.count > 0){
        for(int i = 0; i <= N.count; i++){
            if (N.note[i].empty()==false){
                out << N.note[i] << endl;
            }
        }
        out << endl;
    }
    else out << "Notebook is empty" << endl;
    return out;
}

int main(){
    Notebook N1("Andrey", "k");
    Notebook N2("Sasha", "d");
    Notebook N3;
    Notebook N4("Andrey", "efeeg");
    string s;

    N3 = N1 + N2;
    N3.add("dfd");
    N3.add("dfd");
    N3.del("dfd");
    N4 = N4 + N1;
    N1.add("ssssssssssssssssssssss4");
    cout << "1) Andrey efeeg Andrey k" << endl;
    N4.print_name("Andrey", cout);
    cout << "2) Andrey k Sasha d dfd" << endl;
    cout << N3;
    cout << "3) Andrey k ssssssssssssssssssssss4" << endl;
    cout << N1;
    cout << "4) Sasha d dfd" << endl;
    N3.print_name("Sasha", cout);
    N3 = N3 + N1;
    cout << "5) Andrey k Sasha d dfd Andrey k ssssssssssssssssssssss4" << endl;
    cout << N3;
    cout << "6) Sasha d dfd" << endl;
    N3.print_name("Sasha", cout);
    return 0;
}
        
        