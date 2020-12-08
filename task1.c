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
    int i = 1, fd[2];
    int new_out, old_in = dup(0), old_out = dup(1);

    if (fork()==0){
        execlp(argv[i], argv[i], NULL);
    }
    else wait(NULL);
    i++;
    if (fork() == 0){
        pipe(fd);
        dup2(fd[1], 1);
        close(fd[1]);
        execlp(argv[i], argv[i], NULL);
    }
    else {
        wait(NULL);
        close(0);
        dup2(fd[0], 0);
        close(fd[0]);
        i++;
    }
    close(1);
    close(fd[1]);
    dup2(old_out, 1);
    close(old_out);
    if ((new_out = open(argv[argc-1], O_WRONLY|O_TRUNC|O_CREAT, 0644)) == -1) perror("file");
    dup2(new_out, 1);
    close(new_out);
    if (fork() == 0){
        execlp(argv[i], argv[i], NULL);
    }
    else wait(NULL);
    close(fd[1]); close(fd[0]);
    close(0); close(1);
    dup2(old_out, 1); dup2(old_in, 0);
    close(old_out); close(old_in);
}