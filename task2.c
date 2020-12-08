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

int main(int argc, char** argv){
    int fille, count = 0;
    int c;
    fille = open(argv[argc-1], O_RDWR, 0644);
    read(fille, c, 1);
    while (c != EOF){
        count = 0;
        while (c != '\n'){
            if (issnum(c)){
                count++;
            }
        }
        if (count >= 2){
            
        }
    }
}