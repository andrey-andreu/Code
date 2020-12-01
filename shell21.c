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
char*** masmas = NULL;
char** mas = NULL;
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

char** free_mas(void){
    int j = 0;
    if (mas){
        while (mas[j] != NULL){
            free(mas[j]);
            mas[j] = NULL;
            j++;
        }
        free(mas);
        mas = NULL;
    }
return mas;
}

char*** free_masmas(void){
    int i = 0, j = 0;
    if (masmas){
        while (masmas[i] != NULL){
            while (masmas[i][j] != NULL){
                free(masmas[i][j]);
                masmas[i][j] = NULL;
                j++;
            }
        }
    }
return masmas;
}

void free_masfile(int k){
    int i;
    for (i = 0; i < k; i++){
        if (masfile[i]) free(masfile[i]);
        masfile[i] = NULL;
    }
    free(masfile);
    masfile = NULL;
}

void free_mascom(int l){
    int i = 0, j = 0;
    for (i = 0; i <= l; i++){
        j = 0;
        while (mascom[i][j] != NULL){
            free(mascom[i][j]);
            mascom[i][j] = NULL;
            j++;
        }
        free(mascom[i]);
    }
    if (mascom) free(mascom);
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
    int c, j = 0, i = 0, flag = 0, len = 0, k = 0, l = 0, err = 0, first = 1, next = 0, flagf = 0, p = 0;
    int old_stcount = -1;
    char* vv = NULL;
    char* word = NULL;
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
        i = 0;
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
                    if (c == ' ') next = 0;
                }
            }
            if (j != 0) {
                word[j] = '\0';
                if (strcmp(word, "exit") == 0) flag = 1;
                    mas = realloc(mas, sizeof(char*)*(i+1));
                    mas[i] = malloc(strlen(word)+1);
                    strcpy(mas[i], word);
            }
            if (flagf == 0){
                i++;
                mas = realloc(mas, sizeof(char*)*(i + 1));
                mas[i] = NULL;
                mascom = realloc(mascom, sizeof(*mascom)*(stcount + 1));
                mascom[stcount] = mas;
                if (old_stcount != stcount){
                    masfile = realloc(masfile, sizeof(masfile)*(stcount + 1));
                    masfile[stcount] = NULL;
                }
            }
            else{
                masfile = realloc(masfile, sizeof(masfile)*(stcount + 1));
                masfile[stcount] = malloc(strlen(word)+1);
                strcpy(masfile[stcount], word);
                old_stcount = stcount;
                flagf = 0;
                flagV = 0;
            } 
            vv = realloc(vv, sizeof(char)*(stcount + 1));
            //printf("%d ", stcount);
            vv[stcount] = v;
            /*printf("%c", vv[stcount]);
            printf("%s ", masfile[stcount]);
            printf("%s\n", mascom[stcount][0]);*/
            if (fstikc){
                stcount++;
                mas = NULL;
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
            mas = NULL;
            if (mascom != NULL) free_mascom(stcount);
            mascom = NULL;
            masfile = NULL;
            exit(0);
        }
        printf("!!%d!!\n!!", flagV2);
        if (mascom != NULL){
            for (p = 0; p <= stcount; p++){
                if (mascom[p] != NULL){
                    if (strcmp(mascom[p][0], "cd") == 0){
                        if (mascom[p][1] == NULL) chdir(strdup(getenv("HOME")));
                        else if (chdir(mascom[p][1]) == -1) perror("Child's error");
                    }
                    else{
                        if (masfile) 
                            if (masfile[p]) vcom(vv[p], masfile[p]);
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
                        if (masfile) 
                            if (masfile[p]) vclose(vv[p], old_out, old_in);;
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
        mas = NULL;
        if (vv) free(vv);
        vv = NULL;
        if (mascom != NULL) free_mascom(stcount);
        if (masfile != NULL) free_masfile(stcount);
        mascom = NULL;
        masfile = NULL;
        i = 0;
        k = 0;
        v = '0';
        stcount = 0;
        fstikc = 0;
        first = 1;
    }
    if (c == EOF) printf("%s", "exit\n");
    return 0;
}