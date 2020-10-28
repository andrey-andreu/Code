#include <stdio.h>
#include <string.h> 
#include <stdlib.h>

int main(int argc, char* argv[]){
    FILE *f1 = stdin;
    FILE *f2 = stdout;

    int notstinp = 1, notstout = 1;

    if(argc > 1) {

        if((f1 = fopen(argv[1], "r")) == NULL) {
            fprintf(stderr, "Cannot open input file\n");
            return 1;
        }
        else {notstinp = 0;}
        if(argc > 2)
            if((f2 = fopen(argv[2], "w")) == NULL) {
                fprintf(stderr, "Cannot open output file\n");
                return 2;
            }
            else {notstout = 0;}
    }

    unsigned char *u8 = NULL;
    unsigned short utf16ch;
    if (fread(&utf16ch, sizeof utf16ch, 1, f1) != 0){
        if (utf16ch == 0xFEFF){
                while (fread(&utf16ch, sizeof utf16ch, 1, f1) != 0){
                    if (utf16ch & 0xFF00){
                        if (utf16ch & 0xF800){
                            u8 = malloc(3);
                            u8[0] = (char)(utf16ch >> 12) | 0xE0;
                            u8[1] = ((char)(utf16ch>>6) & 0x3F) | 0x80;
                            u8[2] = ((char)(utf16ch) & 0x3F) | 0x80;
                        }
                        else {
                            u8 = malloc(2);
                            u8[0] = (char)(utf16ch >> 6) | 0xC0;
                            u8[1] = ((char)(utf16ch) & 0x3F) | 0x80;
                        }
                    }
                    else{
                        u8 = malloc(1);
                        u8[0] = (char)(utf16ch);
                    }
                    if ((fwrite(u8, 1,strlen(u8) ,f2)) != strlen(u8)){exit(2);}
                    free(u8); u8 = NULL; utf16ch = 0;
                }
        }
        else {
            if (utf16ch != 0xFFFE) {
                utf16ch = (utf16ch >> 8) | (utf16ch << 8);
                if (utf16ch & 0xFF00){
                    if (utf16ch & 0xF800){
                        u8 = malloc(3);
                        u8[0] = (char)(utf16ch >> 12) | 0xE0;
                        u8[1] = ((char)(utf16ch>>6) & 0x3F) | 0x80;
                        u8[2] = ((char)(utf16ch) & 0x3F) | 0x80;
                        }
                        else {
                            u8 = malloc(2);
                            u8[0] = (char)(utf16ch >> 6) | 0xC0;
                            u8[1] = ((char)(utf16ch) & 0x3F) | 0x80;
                        }
                    }
                else{
                    u8 = malloc(1);
                    u8[0] = (char)(utf16ch);
                }
                if ((fwrite(u8, 1,strlen(u8) ,f2)) != strlen(u8)){exit(2);}
                free(u8); u8 = NULL; utf16ch = 0;
            }
            while (fread(&utf16ch, sizeof utf16ch, 1, f1) != 0){
		    utf16ch = (utf16ch >> 8) | (utf16ch << 8);
                if (utf16ch & 0xFF00){
                    if (utf16ch & 0xF800){
                        u8 = malloc(3);
                        u8[0] = (char)(utf16ch >> 12) | 0xE0;
                        u8[1] = ((char)(utf16ch>>6) & 0x3F) | 0x80;
                        u8[2] = ((char)(utf16ch) & 0x3F) | 0x80;
                    }
                    else {
                        u8 = malloc(2);
                        u8[0] = (char)(utf16ch >> 6) | 0xC0;
                        u8[1] = ((char)(utf16ch) & 0x3F) | 0x80;
                    }
                }
                else{
                u8 = malloc(1);
                u8[0] = (char)(utf16ch);
                }
                if ((fwrite(u8, 1,strlen(u8) ,f2)) != strlen(u8)){exit(2);}
                free(u8); u8 = NULL; utf16ch = 0;
            }
        }
    }
    else {fprintf (stderr, "%s: file is empty %s\n", argv[0], *argv);}
    if(feof(f1) && utf16ch) {
        fprintf (stderr, "%s: file has odd count of bits %s\n", argv[0], *argv);
        exit(4);
    }
if (notstinp) fclose(f1);
if (notstout) fclose(f2);
return 0;      
}
