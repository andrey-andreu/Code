#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
    FILE *f1 = stdin;
    FILE *f2 = stdout;

    int stinp = 1, stout = 1;

    if(argc > 1) {

        if((f1 = fopen(argv[1], "r")) == NULL) {
            fprintf(stderr, "Cannot open input file\n");
            return 1;
        }
        else {stinp = 0;}
        if(argc > 2)
            if((f2 = fopen(argv[2], "w")) == NULL) {
                fprintf(stderr, "Cannot open output file\n");
                return 2;
            }
            else {stout = 0;}
    }

    int c, count = 0;
    char utf8ch;
    unsigned short u16;

    while (fread(&utf8ch, sizeof utf8ch, 1, f1) != 0){
        count++;
        if (utf8ch & 0x80){
            if ((utf8ch & 0xC0) == 0xC0){
                if((utf8ch & 0xF0) == 0xE0){
                    u16 = (unsigned short)(utf8ch & 0x0F) << 12;
                    c = fread(&utf8ch, sizeof utf8ch, 1, f1);
                    if ((c) && ((utf8ch & 0xC0) == 0x80)){
                        count++;
                        u16 = u16 | ((unsigned short)(utf8ch & 0x3F) << 10);
                    }
                    else {fprintf(stderr, "неверный байт %x в позиции %d\n", (unsigned)utf8ch, count);}
                    c = fread(&utf8ch, sizeof utf8ch, 1, f1);
                    if ((c) && ((utf8ch & 0xC0) == 0x80)){
                        count++;
                        u16 = u16 | (unsigned short)(utf8ch & 0x3F);
                        u16 = (u16 >> 8) | (u16 << 8);
                        if (!(fwrite(&u16, 1, sizeof u16 ,f2))){exit(2);}
                    }
                    else {fprintf(stderr, "неверный байт %x в позиции %d\n", (unsigned)utf8ch, count);}
                }
                else if ((utf8ch & 0xE0) == 0xC0) {
                    u16 = (unsigned short)(utf8ch & 0x1F) << 6;
                    c = fread(&utf8ch, sizeof utf8ch, 1, f1);
                    if ((c) && ((utf8ch & 0xC0) == 0x80)){
                        count++;
                        u16 = u16 | (unsigned short)(utf8ch & 0x3F);
                        u16 = (u16 >> 8) | (u16 << 8);
                        if (!(fwrite(&u16, 1, sizeof u16 ,f2))){exit(2);}
                    }
                    else {fprintf(stderr, "неверный байт %x в позиции %d\n", (unsigned)utf8ch, count);}
                }
                else {fprintf(stderr, "неверный байт %x в позиции %d\n", (unsigned)utf8ch, count);}
            }
        }
        else {
            u16 = (unsigned short)(utf8ch);
            u16 = (u16 >> 8) | (u16 << 8);
            if (!(fwrite(&u16, 1, sizeof u16 ,f2))){exit(2);}
        }
    }
if (stinp) fclose(f1);
if (stout) fclose(f2);
return 0;
}