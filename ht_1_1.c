#include <stdio.h>
int main()
{
        const int size = 1024;
        int n = 0, t, j, i, d, a[size];
        for (i = 0; i < size; ++i){
                if (scanf("%d", &d) == 1){
                a[i] = d;
                n = i;}}
        for (i = 0 ; i < n; ++i){
                for(j = 0; j < n - i; ++j){
                         if(a[j] > a[j+1]){
                                 t = a[j];
                                 a[j] = a[j+1];
                                 a[j+1] = t;}}}
        for (i = 0; i <= n; ++i){
                printf("%d ", a[i]);}
        return 0;
}