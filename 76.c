#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define SIZE 1024
int main(int argc, char *argv[])
{
        int flag = 1;
        char *line2 = malloc(SIZE), *line1 = malloc(SIZE);
        FILE *fp1 = fopen(argv[1], "r");
        FILE *fp2 = fopen(argv[2], "r");

        line1 = fgets(line1, SIZE, fp1);
        line2 = fgets(line2, SIZE, fp2);
        while ((line1 != NULL) && (line2 != NULL)){
                if (strcmp(line1, line2) != 0){
                        printf("%s", line1);
                        printf("%s", line2);
                        flag = 0;
                        break;
                }
                line1 = fgets(line1, SIZE, fp1);
                line2 = fgets(line2, SIZE, fp2);
        }
        if (flag){
                if ((line1 == NULL) && (line2 != NULL)){
                        puts("EOF");
                        printf("%s", line2);
                }
                else if ((line2 == NULL) && (line1 != NULL)){
                        printf("%s", line1);
                        puts("EOF");
                }
                else puts("files are equal");
        }
return 0;
}