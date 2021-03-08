#include <iostream>
#include <math.h>
using namespace std;

class Vector_new{
    private: 
        int size;
        int* body;
    public:
        Vector_new(){size = 0; body = NULL;}
        Vector_new(int s){
            if (s < 0) perror("Bad size");
            body = new int [s];
            size = s;
        }

        Vector_new(const Vector_new& v){
            size = v.size;
            body = new int [v.size];
            for (int i = 0; i < v.size; i++){
                body[i] = v.body[i];
            }
        }  

        ~Vector_new(){
            delete[] body;
        }

        int get_size(){return size;}

        int norm(){
            int norm = 0;
            for (int i = 0; i < size; i++){
                norm += body[i]*body[i];
            }
            return sqrt(norm);
        }

        int& operator[] (int ind){
            if (size <= ind || ind < 0) perror("Out of range");
            return body[ind];
        }

        void do_vector(const string& s){
            if (size != s.size()) perror("Size is different");
            for (int i = 0; i < size; i++){
                body[i] = int(s[i] - '0');
            }
        }

        Vector_new& operator= (const Vector_new& v){
            if (this!=&v){
                size = v.size;
                body = new int [size];
                for (int i = 0; i < size; i++){
                    body[i] = v.body[i];
                }
            }
            return *this;
        }
        friend ostream& operator<<(ostream& out, const Vector_new& v);

        const Vector_new& operator+=(const Vector_new& v){
            if (size == v.size){
                for (int i = 0; i < v.size; i++){
                    body[i] += v.body[i];
                }
            }
            else perror ("Size is different");
            return *this;
        }

        const Vector_new& operator+(const Vector_new& v) {return Vector_new(*this) += v;}

        const Vector_new& operator-=(const Vector_new& v){
            if (size == v.size){
                for (int i = 0; i < v.size; i++){
                    body[i] -= v.body[i];
                }
            }
            else perror ("Size is different");
            return *this;
        }

        const Vector_new& operator-(const Vector_new& v) {return Vector_new(*this) -= v;}

        const Vector_new& operator*=(const Vector_new& v){
            if (size == v.size){
                for (int i = 0; i < v.size; i++){
                    body[i] *= v.body[i];
                }
            }
            else perror ("Size is different");
            return *this;
        }

        const Vector_new& operator*(const Vector_new& v) {return Vector_new(*this) *= v;}

        friend bool operator< (Vector_new& v1, Vector_new& v2);
        friend bool operator<= (Vector_new& v1, Vector_new& v2);
        friend bool operator> (Vector_new& v1, Vector_new& v2);
        friend bool operator>= (Vector_new& v1, Vector_new& v2);
        friend bool operator== (Vector_new& v1, Vector_new& v2);
};
    bool operator< (Vector_new& v1, Vector_new& v2){
        if (v1.get_size() != v2.get_size()) perror("Size is different");   
        return v1.norm() < v2.norm();
    }

    bool operator<= (Vector_new& v1, Vector_new& v2){
        if (v1.get_size() != v2.get_size()) perror("Size is different");   
        return v1.norm() <= v2.norm();
    }

    bool operator> (Vector_new& v1, Vector_new& v2){
        if (v1.get_size() != v2.get_size()) perror("Size is different");   
        return v1.norm() > v2.norm();
    }

    bool operator>= (Vector_new& v1, Vector_new& v2){
        if (v1.get_size() != v2.get_size()) perror("Size is different");   
        return v1.norm() >= v2.norm();
    }

    bool operator== (Vector_new& v1, Vector_new& v2){
        if (v1.size != v2.size) perror("Size is different");   
        return v1.norm() == v2.norm();
    }

    ostream& operator<<(ostream& out, Vector_new& v){
        if (v.get_size() > 0){
            cout << "( ";
            for (int i = 0; i < v.get_size()-1; i++){
                cout << v[i] << ", ";
            }
            cout << v[v.get_size()-1] << " )";
        }
        else{
            cout << " Vector is emty";
        }
    return out;
    }


int main() {
    Vector_new V1(3);
    Vector_new V2(V1);
    Vector_new V3(3);
    Vector_new V4;

    //V1.do_vector("111");
    for (int i = 0; i < V1.get_size(); i++){
        V1[i] = 1;
    }
    cout << V2 << endl;
    cout << (V1 == V2) << endl;
    for (int i = 0; i < V3.get_size(); i++){
        V3[i] = 3;
    }
    cout << V3 << endl;
    V1 += V3;
    V4 = V3 + V3;
    cout << V1 << endl;
    cout << V4 << endl;
    V4 = V3 * V3;
    cout << V4 << endl;
    cout << V3 << endl;
    return 0;
}
