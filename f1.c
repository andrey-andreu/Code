#include <stdio.h>
#include <stdlib.h>
char *mas1 = NULL;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char *mas2 = NULL;
int main()
{
        int num2 = 0, i, c, num1 = 0, size = 0;
        while ((c = getchar()) != EOF){
                while (c != '\n'){
                        if (num2 == size){
                                size = 2*size + 1;
                                mas2 = realloc(mas2, size);
                        }
                        mas2[num2] = c;
                        num2++;
                        if ((c = getchar()) == EOF) break;
                }
                if (num2 > num1){
                        if (mas1 != NULL) free(mas1);
                        mas1 = mas2;
                        num1 = num2;
                }
                else free(mas2);

                mas2 = NULL;
                num2 = 0;
                size = 0;
        }
        for (i = 0; i < num1; i++)
                putchar(mas1[i]);
        putchar('\n');
        if (mas1 != NULL) free(mas1);
        if (mas1 == NULL) return 1;
        else return 0;
}