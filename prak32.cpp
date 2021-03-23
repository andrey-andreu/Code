#include <iostream>
using namespace std;

class Student {
    protected:
        int start_year, student_ID, age, name_count;
        char* name; // для того, чтобы "искуственно" сделать абстракнтный класс неплоским
        static int count; // стчетчик студентов
    public:
        Student(){};
        Student(int year, int ID, int a, string n){
            count++;
            start_year = year;
            student_ID = ID;
            age = a;
            name_count = n.size();
            name = new char[name_count + 1];
            for (int i = 0; i < name_count; i++){
                name[i] = n[i];
            }
            name[name_count] = 0;
        }
        Student(const Student& s){
            int i = 0;
            start_year = s.start_year;
            student_ID = s.student_ID;
            age = s.age;
            name_count = s.name_count;
            name = new char[name_count];
            for (int i = 0; i < name_count; i++){
                name[i] = s.name[i];
            }
        }

        // печать количества студентов
        void print_count(ostream& out){ 
            out << "Count of students = " << count << endl;
        }

        // константный 'геттер' для количества студентов
        int get_count() const {return count;}

        string get_name()const {
            string n(const name);
            
            //for (int i = 0; i < name_count; i++){
                //n.push_back(name[i]);
            //}
            return n;
        }

        virtual ~Student(){
            delete [] name;
            count--;
        }

        // у бакалавров формула подсчета года выпуска start_year + 4, у магистров start_year + 2
        virtual int finish_year(int s) = 0;

        // у бакалавров и магистров будут добавленые еще разные дополнительные переменные 
        virtual void print_dop(ostream& out) = 0;

        friend ostream& operator<<(ostream& out, Student *s){
            int finish;
            finish = s->finish_year(s->start_year);
            out << "Student's name: ";
            for (int i = 0; i < s->name_count; i++){
                out << s->name[i];
            }
            out << endl;
            out << " Start year: ";
            out << s->start_year << endl;
            out << " Finish year: ";
            out << finish << endl;
            out << " student_ID: ";
            out << s->student_ID << endl;
            out << " Age: ";
            out << s->age << endl;
            s->print_dop(out);
            out << endl;
            return out;
        }

        Student& operator= (const Student& s){
            if (this!=&s){
                start_year = s.start_year;
                student_ID = s.student_ID;
                age = s.age;
                name_count = s.name_count;
                name = new char[name_count];
                for (int i = 0; i < name_count; i++){
                    name[i] = s.name[i];
                }
            }
            return *this;
        }

};

int Student::count = 0; // обнуляем счетчик студентов

// далее студентов будем подразделять на бакаловров и магистров: у первых добавляется средний балл,
// у вторых тема работы

class Bakalavr: public Student{
    float average_score;
    public: 
        Bakalavr(){};

        Bakalavr(int year, int ID, int a, string n, float score)
            : Student(year, ID, a, n), average_score(score){}

        Bakalavr(const Bakalavr& b) 
            : Student(b), average_score(b.average_score){}
        
        void print_dop(ostream& out){
            out << " Average_score: " << average_score;
        }

        int finish_year(int s){ return s+4;}

        Bakalavr& operator= (const Bakalavr& s){
            if (this!=&s){
                start_year = s.start_year;
                student_ID = s.student_ID;
                age = s.age;
                name_count = s.name_count;
                name = new char[name_count];
                for (int i = 0; i < name_count; i++){
                    name[i] = s.name[i];
                }
                average_score = s.average_score;
            }
            return *this;  
        }
        ~Bakalavr(){};

};

class Magistr: public Student{
    string topic;
    public:
        Magistr(){};
        Magistr(int year, int ID, int a, string n, string t)
            : Student(year, ID, a, n), topic(t){}

        Magistr(const Magistr& m) 
            : Student(m), topic(m.topic){}
        
        void print_dop(ostream& out){
            out << " Topic: " << topic;
        }

        int finish_year(int s){ return s+2;}

        Magistr& operator= (const Magistr& s){
            if (this!=&s){
                start_year = s.start_year;
                student_ID = s.student_ID;
                age = s.age;
                name_count = s.name_count;
                name = new char[name_count];
                for (int i = 0; i < name_count; i++){
                    name[i] = s.name[i];
                }
                topic = s.topic;
            }
            return *this;  
        }
        ~Magistr(){};
};

int main(){
    Student *M;
    M = new Magistr(2001, 1234567, 22, "Fill", "math");
    Student *B;
    B = new Bakalavr(2019, 2345678, 20, "BBooooooooob", 4.76);
    Student *B2;
    B2 = new Bakalavr(2019, 2345678, 20, "BBooooooooob", 4.76);

    


    cout << M << endl;
    cout << B << endl; 
    delete B2;
    M->print_count(cout);
    int c;
    c = M->get_count(*M);
    cout << endl << endl << c;
    cout << endl << endl << M->get_name() << endl;
    Student *M1;
    M1 = M;
    cout << M1 << endl;
    delete B; delete M;
}
