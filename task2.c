#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <limits.h>
#include <fcntl.h>
#include <wait.h>
#include <signal.h>
#include <ctype.h>

int main(int argc, char** argv){
    int fille1, fille2, count = 0, count_str, flag = 0, symb = 0, symb2 = 0, i = 0, k = 0;
    char c;
    fille1 = open(argv[1], O_RDONLY);
    fille2 = open(argv[1], O_WRONLY);

    while (read(fille1, &c, sizeof(char)) > 0){
        count = 0;
        flag = 0;
        symb++;
        symb2 = 0;
        while (c != '\n'){
            symb2++;
            if (((c - '0') >= 0) && ((c - '9') <= 0)){
                count++;
            }
            read(fille1, &c, sizeof(char));
        }
        if (count == 2){
            lseek(fille1, symb-symb2, SEEK_SET);
            i = 0;
            for (i = 0; i <= symb2; i++){
                read(fille1, &c, sizeof(char));
                write(fille2, &c, sizeof(char));
            }
        }
    }
    ftruncate(fille2, symb);
    close(fille1);
    close(fille2);
}