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

#define LEN 10

#define RESET   "\033[0m"
#define YELLOW  "\033[1;35m"
#define BLUE  "\033[1;34m"
#define WHITE   "\033[1;37m"
#define RED     "\033[1;31m"

char*** mascom = NULL;
char** masfile = NULL;
int new_out;
int new_in;
int flagV = 0, flagV2 = 0, fstikc = 0, stcount = 0, next = 0, flagfile = 0, fd[2], one = 1;

void printdir(void){
    char* path = NULL;
    char buf[PATH_MAX];
    if ((path = getcwd(buf, PATH_MAX)) != NULL){ 
        printf("%s%s%s", YELLOW, buf, RESET);
        printf("%s:%s", WHITE,  RESET);
        printf("%s~%s", BLUE,  RESET);
        printf("%s$ %s", WHITE,  RESET);
    }
    else perror("Getcwd's error");
}

void cdcom(char **mas){
    if (mas[1] == NULL) chdir(strdup(getenv("HOME")));
    else if (chdir(mas[1]) == -1) perror("Child's error");
}

void free_masfile(int k){
    int i;
    for (i = 0; i <= k; i++){
        if (masfile[i] != NULL){ 
            free(masfile[i]);
            masfile[i] = NULL;
        }
    }
    free(masfile);
    masfile = NULL;
}

void free_mascom(int l, int* r){
    int i = 0, j = 0;
    for (i = 0; i <= l; i++){
        for (j = 0; j <= r[i]; j++){
            free(mascom[i][j]);
            mascom[i][j] = NULL;
        }
        free(mascom[i]);
        mascom[i] = NULL;
    }
    if (mascom) free(mascom);
    mascom = NULL;
}

void vcom(char v, char *masfile){
    if ((v == '>') && (flagV2 == 0)){
        if ((new_out = open(masfile, O_WRONLY|O_TRUNC|O_CREAT, 0644)) == -1) perror("file");
        close(1);
        dup2(new_out, 1);
        close(new_out);
    }
    if (v == '<'){
        if ((new_in = open(masfile, O_RDONLY)) == -1) perror("file");
        close(0);
        dup2(new_in, 0);
        close(new_in);
    }
    if (flagV2 && (v == '>')){
        if ((new_out = open(masfile, O_WRONLY|O_APPEND|O_CREAT, 0644)) == -1) perror("file");
        close(1);
        dup2(new_out, 1);
        close(new_out);
        flagV2 = 0;
    }
    flagV = 0;
}

void vclose(char v, int old_out, int old_in){
    if (v == '>'){
        close(1);
        dup2(old_out, 1);
        close(old_out);
        old_out = dup(1);
    }
    else if (v == '<'){
        close(0);
        dup2(old_in, 0);
        close(old_in);
        old_in = dup(0);
    }
}

void ret_in(int old_in){
    close(0);
    dup2(old_in, 0);
    close(old_in);
    old_in = dup(0);
}

void ret_out(int old_out){
    close(1);
    dup2(old_out, 1);
    close(old_out);
    old_out = dup(1);
}

int main(int argc, char* argv[]){
    int c, j = 0, i = 0, flag = 0, len = 0, err = 0, first = 1, flagf = 0, p = 0;
    char* vv = NULL;
    int old_stcount = -1;
    char* word = NULL;
    int *lencom = NULL;
    char v = '0';
    int old_in = dup(0);
    int old_out = dup(1);
    int fd[2];

    printdir();
    c = getchar();
    while (c != EOF){
        while (c == '\n'){ 
            printdir();
            c = getchar();
        } 
        if (c == EOF) break;
        i = 0;
        lencom = malloc(sizeof(int) + 1);
        lencom[0] = 0;
        mascom = malloc(sizeof(char**) + 2);
        mascom[0] = NULL;
        masfile = malloc(sizeof(char*) + 2);
        masfile[0] = NULL;
        while ((c != '\n') && (c != EOF)){
            while (c == ' ') c = getchar();
            if (c == '"'){
                len = LEN;
                word = malloc(len);
                j = 0;
                while ((c = getchar()) != '"'){
                    if (c == '\n'){
                        fprintf(stderr, "%s%s%c%s%s\n", "You lost this ", RED, '"', RESET, " ,be careful");
                        break;
                    }
                    word[j++] = c; 
                    if (j >= len){
                        len += LEN;
                        word = realloc(word, len);
                    }
                }
                if (c != '\n') c = getchar();
            }
            else{ 
                len = LEN;
                word = malloc(len);
                j = 0;
                while ((c != ' ') && (c != '\n') && (c != EOF)){
                    if ((c == '>') || (c == '<')){
                        v = c;
                        flagV = 1;
                        if (((c = getchar()) == '>') && (v == '>')) {
                            flagV2 = 1;
                            c = getchar();
                        }
                        if ((c == '>') || (c == '<')) {
                            perror("To many '<' or '>'");
                            while ((c == '>') || (c == '<')) c = getchar();
                            err = 1;
                        }
                        break;
                    }
                    if (c == '|'){
                        fstikc = 1;
                        c = getchar();
                        if (c == '|'){
                            perror("To many '|'");
                            while (c == 'l') c = getchar();
                            err = 1;
                        }
                        break;
                    }
                    word[j++] = c;
                    if (j >= len-1){
                        len += LEN;
                        word = realloc(word, len);
                    }
                    c = getchar();
                }
            }

            if (j != 0) {
                word[j] = '\0';
                if (strcmp(word, "exit") == 0) flag = 1;
                mascom[stcount] = realloc(mascom[stcount], sizeof(char*)*(i+2));
                mascom[stcount][i] = malloc(strlen(word)+1);
                strcpy(mascom[stcount][i], word);
            }
            if (flagf == 0){
                i++;
                mascom[stcount] = realloc(mascom[stcount], sizeof(char*)*(i + 2));
                mascom[stcount][i] = NULL;
                if (old_stcount != stcount){
                    masfile = realloc(masfile, sizeof(char*)*(stcount + 2));
                    masfile[stcount] = NULL;
                }
            }
            else{
                masfile = realloc(masfile, sizeof(char*)*(stcount + 2));
                masfile[stcount] = NULL;
                if (j != 0) masfile[stcount] = malloc(strlen(word)+2);
                strcpy(masfile[stcount], word);
                old_stcount = stcount;
                flagf = 0;
                flagV = 0;
            }
            vv = realloc(vv, sizeof(char)*(stcount + 2));
            vv[stcount] = v;
            lencom[stcount] = i;
            if (fstikc){
                stcount++;
                lencom = realloc(lencom, sizeof(int)*(stcount + 2));
                lencom[stcount] = 0;
                mascom = realloc(mascom, sizeof(char**)*(stcount + 2));
                mascom[stcount] = NULL;
                i = 0;
                fstikc = 0;
            }
            if (flagV) flagf = 1;
            if (word) free(word);
            word = NULL;
            while (c == ' ') c = getchar();
        }
        flagf = 0;
        if (flag){
            if (vv) free(vv);
            vv = NULL;
            if (mascom != NULL) free_mascom(stcount, lencom);
            if (masfile != NULL) free_masfile(stcount);
            free(lencom);
            lencom = NULL;
            mascom = NULL;
            masfile = NULL;
            exit(0);
        }
        if (mascom != NULL){
            for (p = 0; p <= stcount; p++){
                if (mascom[p] != NULL){
                    if (strcmp(mascom[p][0], "cd") == 0){
                        if (mascom[p][1] == NULL) chdir(strdup(getenv("HOME")));
                        else if (chdir(mascom[p][1]) == -1) perror("Child's error");
                    }
                    else{
                        if (masfile != NULL) 
                            if (masfile[p] != NULL) vcom(vv[p], masfile[p]);
                        if(!first) {
                            first = 1;
                            close(0);
                            dup2(fd[0], 0);
                            close(fd[0]);
                        }
                        if(p < stcount) {
                            first = 0;
                            pipe(fd);
                            close(1);
                            dup2(fd[1], 1);
                            close(fd[1]);
                        }
                        if (fork() == 0){
                            execvp(mascom[p][0], mascom[p]);
                            perror("Execvp's error");
                        }
                        else wait(NULL);
                        if (masfile != NULL) 
                            if (masfile[p] != NULL) vclose(vv[p], old_out, old_in);;
                    }
                }
                if (stcount){
                    close(1);
                    dup2(old_out, 1);
                    close(old_out);
                    old_out = dup(1);
                    close(0);
                    dup2(old_in, 0);
                    close(old_in);
                    old_in = dup(0);
                }
            }
        }
        if (vv) free(vv);
        vv = NULL;
        if (mascom != NULL) free_mascom(stcount, lencom);
        if (masfile != NULL) free_masfile(stcount);
        free(lencom);
        lencom = NULL;
        mascom = NULL;
        masfile = NULL;
        i = 0;
        v = '0';
        stcount = 0;
        old_stcount = -1;
        fstikc = 0;
        first = 1;
    }
    if (c == EOF) printf("%s", "exit\n");
    return 0;
}