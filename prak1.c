#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX 32

int allcount = 0;

struct tnode {
 char *word;
 int count;
 double fr;
 struct tnode *left;
 struct tnode *right;
};

void treeprint(struct tnode *p)
{
    int i=0, k=0, flag1=1;
    char* word1=NULL;
    word1 = malloc(MAX);
    if (p != NULL) {
        treeprint(p->left);
	while (flag1){
		i = 0;
		while(p->word[k] != '/0'){
			word1[i] = p->word[k];
			i++;
			k++;
		}
		word1[i] = p->word[k];
		printf("%s %4d %f\n" ,word1, p->count, p->fr);
		if ((p->word[k+1] == ' ') && (p->word[k+2] == ' ')) flag1 = 0;
		else k++;
		if (word1 != NULL) free(word1);
	}
        treeprint(p->right);
    }
};

void doend(struct tnode *t)
{
	if (t != NULL){
		if (t->left != NULL)
			doend(t->left);
		t->left->word = realloc(t->left->word, sizeof(t->left->word) + 2);
		t->left->word[sizeof(t->left->word)-2] = ' ';
		t->left->word[sizeof(t->left->word)-1] = ' ';
		if (t->right != NULL)
			doend(t->right);
		t->right->word = realloc(t->right->word, sizeof(t->right->word) + 2);
		t->right->word[sizeof(t->right->word)-2] = ' ';
		t->right->word[sizeof(t->right->word)-1] = ' ';
	}
}

struct tnode *talloc(void)
{
    return (struct tnode *) malloc(sizeof(struct tnode));
};


char *strdup1(char *s)
{
    char *p;
    p = (char *) malloc(strlen(s)+1);
    if (p != NULL)
    	p = strcpy(p, s);
    return p;
};

struct tnode *addtree(struct tnode *p, char *w)
{
    int cond;
    if (p == NULL) {
        p = talloc();
        p->word = strdup1(w);
        p->count = 1;
        p->left = p->right = NULL;
    }
    else if ((cond = strcmp(w, p->word)) == 0)
        p->count++;
    else if (cond < 0)
        p->left = addtree(p->left, w);
    else
        p->right = addtree(p->right, w);
    return p;
};

void deltree(struct tnode *t)
{
    if (t != NULL){
        deltree (t->left);
        deltree (t->right);
        free (t->word);
        free (t);
    }
}

struct tnode* dotree(void)
{
    char *word = NULL;
    char *punct = NULL;
    struct tnode *root = NULL;
    int i = 0;
    int c;
    while((c = getchar()) != EOF) {
        while(isalnum(c)) {
		if(word == NULL) 
            word = malloc(MAX);
        word[i] = c;
        i++;
        c = getchar();
	    }
	    if (word != NULL){
           	word[i] = '\0';
		    i = 0;
            allcount++;
            root = addtree(root, word);
            free(word);
            word = NULL;
	    }
	    if(ispunct(c)){
    		punct = malloc(sizeof(char) * 2);
            punct[0] = c;
            punct[1] = '\0';
            allcount++;
            root = addtree(root, punct);
            free(punct);
            punct = NULL;
	    }
    }
return root;
};

struct tnode *addtree2(struct tnode *tr, struct tnode *p)
{
    int size;
    if (tr == NULL) {
        tr = talloc();
            tr->count = p->count;
            tr->word = strdup1(p->word);
            tr->fr = (double) p->count/allcount;
            p->left = p->right = NULL;
    }
    else if (tr->count == p->count){
	size = sizeof(tr->word) + sizeof(p->word) + 1;
        tr->word = realloc(tr->word, size);
        tr->word[sizeof(tr->word)] = ' ';
        strcpy(tr->word + (sizeof(tr->word)+1), p->word);
    }
    else if (tr->count < p->count)
        tr->left = addtree2(tr->left, p);
    else
        tr->right = addtree2(tr->right, p);
    return tr;
};

void dotree2(struct tnode *p, struct tnode *tr)
{
    if (p != NULL){
        if (p->left != NULL)
            dotree2(p->left, tr);
        tr = addtree2(tr, p->left);
        if (p->right != NULL) 
            dotree2(p->right, tr);
        tr = addtree2(tr, p->right);
    }
}

int main(int argc,char *argv[])
{
    struct tnode *tree = NULL;
    struct tnode *tr = NULL;
    FILE *f1;
    FILE *f2;
 
    f1=stdin;
    f2=stdout;
    if (argc>2){
        if (!strcmp(argv[1],"-i")) f1 = fopen(argv[2],"r");
        if (!strcmp(argv[1],"-o")) f2 = fopen(argv[2],"r");
    tree = dotree();
    dotree2(tree, tr);
    doend(tr);
    treeprint(tr);
    deltree(tr);
    deltree(tree);
    return 0;
}
