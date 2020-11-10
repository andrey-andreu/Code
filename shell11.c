#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <limits.h>
#define RESET   "\033[0m"
#define YELLOW  "\033[1;33m"

void printdir(void){
    char* path = NULL;
    char buf[PATH_MAX];
    if ((path = getcwd(buf, PATH_MAX)) != NULL){ 
        printf("%s%s %s", YELLOW, buf, RESET);
    }
    else perror("Getcwd's error");
}

int main(int argc, char* argv[]){
    int c, j, i = 0, flag = 0;
    char** mas = NULL;
    char* word = NULL;
    char* home;

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
                word = malloc(1);
                j = 0;
                while ((c = getchar()) != '"'){
                    word[j++] = c; 
                    word = realloc(word, j+1);
                }
                c = getchar();
            }
            else{ 
                word = malloc(1);
                j = 0;
                while ((c != ' ') && (c != '\n') && (c != EOF)){
                    word[j++] = c;
                    word = realloc(word, j+1);
                    c = getchar();
                }
            }
            word[j] = '\0';
            if (strcmp(word, "exit") == 0) flag = 1;
            mas = realloc(mas, sizeof(char**)*(i + 1));
            mas[i++] = strdup(word);
            free(word);
            word = NULL;
            while (c == ' ') c = getchar();
        }
        if (flag){
            for (j = 0; j < i; j++){
                        free(mas[j]);
                        mas[j] = NULL;
                    }
            free(mas);
            mas = NULL;
            break;
        }
        if (mas){
            mas[i] = NULL;
            if (strcmp(mas[0], "cd") == 0){
                if (mas[1] == NULL){
                    home = strdup(getenv("HOME"));
                    chdir(home);
                    free(home);
                    home = NULL;
                }
                else if (chdir(mas[1]) == -1) perror("Child's error");
            }
            else{
                if (fork() == 0){
                    execvp(mas[0], mas);
                    perror("Execvp's error");
                }
                else wait(0);
            }
            for (j = 0; j < i; j++){
                        free(mas[j]);
                        mas[j] = NULL;
                    }
            free(mas);
            mas = NULL;
        }
    }
    if (c == EOF) printf("%s", "exit\n");
    return 0;
}
