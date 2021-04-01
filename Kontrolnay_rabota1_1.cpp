#include <iostream>
using namespace std;

class BadSeq{};

class IntSeq{
    int *mas;
    int size;
    public: 
        virtual int Distance() const = 0;
        IntSeq(){}
        IntSeq(int *pSeq, int n){
            size = n;
            mas = pSeq;
            mas = new int[n];
            for (int i =0; i < n; i++){
                mas[i] = pSeq[i];
            }
        }
        ~ IntSeq(){delete[] mas;}
        int get_size() const {return size;}
        int* get_mas() const {return mas;}

};

class BadConcat{
    int a, b, c, d;
    public:
        BadConcat(int a1, int b1, int c1, int d1){a=a1; b=b1; c=c1; d=d1;}
        int GetLeft1() const {return a;}
        int GetRight1() const {return b;}
        int GetLeft2() const {return c;}
        int GetRight2() const {return d;}
};

class OrderSeq: public IntSeq{
    public:
        OrderSeq(int *pSeq, int n): IntSeq(pSeq, n){
            for (int i = 0; i < n - 1; i++){
                if (pSeq[i+1] < pSeq[i]) {throw BadSeq();}
            }
        }

        int Distance() const{
            int max = 0;
            int m = 0;
            for (int i = 0; i < get_size() - 1; i++){
                m = get_mas()[i+1] - get_mas()[i];
                if(m > max){ max = m;}
            }
            return max;
        }

        OrderSeq operator+ (const OrderSeq& seq){
            int s = get_size();
            int size1 = s + seq.get_size();
            int* mas1 = new int [size1];
            for (int i = 0; i < s; i++){
                mas1[i] = get_mas()[i];
            }
            for (int i = s; i < size1; i++){
                mas1[i] = seq.get_mas()[i-s]; // на контрольно работе ошибка была тут, присваивание имело вид: mas1[i] = seq.get_mas()[i];
            }

            if (mas1[s-1] > mas1[s]) {throw BadConcat (mas1[0], mas1[s-1], mas1[s], mas1[size1-1]);}
            return OrderSeq(mas1, size1);
        }

};

int main(){
    try { int a[] = {1,2,3}; int b[] = {7,9,10,13}; int c[] = {5,6};
        OrderSeq sa(a,3), sb(b, 4), sc(c, 2);
        cout << (sa+sb).Distance() << ',' << (sa+sc+sb).Distance() << endl;
        cout << (sb+sc).Distance() << endl;   
    }
    catch (const BadConcat &bad) {cout << "Bad concat:" << bad.GetLeft1() << ',' << bad.GetRight1() << '+' << bad.GetLeft2() << ',' << bad.GetRight2() << endl;}
    catch (BadSeq b) {cout << "Bad sequence\n";}
    return 0;
}

/* 
   вывод:
   4,3
   Bad concat:7,13+5,6
*/
