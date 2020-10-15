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

/*void treeprint(struct tnode *p)
{
    int i=0, k=0, flag1=1;
    char* word1=NULL;
    word1 = malloc(MAX);
    if (p != NULL) {
        treeprint(p->left);
	while (flag1){
		i = 0;
		while(p->word[k] != '\0'){
			word1[i] = p->word[k];
			i++;
			k++;
		}
		word1[i] = p->word[k];
		printf("%s %4d %f\n" ,word1, p->count, p->fr);
		if ((p->word[k+1] == ' ') && (p->word[k+2] == ' ')) flag1 = 0;
		else k++;
		if (word1 != NULL){
			free(word1);
			word1 = NULL;
		}
	}
        treeprint(p->right);
    }
};*/

/*void doend(struct tnode *t)
{
	int s;
	if (t != NULL){
		t->left->word = realloc(t->left->word, sizeof(t->left->word) + 2);
		t->left->word[sizeof(t->left->word)-2] = ' ';
		t->left->word[sizeof(t->left->word)-1] = ' ';
		doend(t->left);
		doend(t->right);
	}
};*/

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
        if (t->word){
 		free (t->word);
		t->word = NULL;
	}
        if (t){
		free (t);
		t = NULL;
	}
    }
};

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
            if (word) free(word);
            word = NULL;
	    }
	    if(ispunct(c)){
    		punct = malloc(sizeof(char) * 2);
            punct[0] = c;
            punct[1] = '\0';
            allcount++;
            root = addtree(root, punct);
            if (punct) free(punct);
            punct = NULL;
	    };
    };
return root;
};

/*struct tnode *addtree2(struct tnode *tr, struct tnode *p)
{
    int size, sizew;
    if (tr == NULL) {
        tr = talloc();
            tr->count = p->count;
            tr->word = strdup1(p->word);
            tr->fr = (double) p->count/allcount;
            p->left = p->right = NULL;
    }
    else if (tr->count == p->count){
	size = sizeof(tr->word) + sizeof(p->word) + 1;
	sizew = sizeof(tr->word);
        tr->word = realloc(tr->word, size);
        tr->word[sizew] = ' ';
        strcpy(tr->word + sizew, p->word);
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
};

void maxcount(struct tnode *t, int *max)
{
    if (t != NULL){
        if (t->count > *max) *max = t->count;
        maxcount(t->right, max);
        maxcount(t->left, max);
    }
};

void printtree(struct tnode *t, int max){
    if (t != NULL){
        printtree(t->left, max);
        printtree(t->right, max);
       if (t->count == max)
            printf("%s %4d %f\n" ,t->word, t->count, (double) t->count/allcount);
    }
};*/

int main(int argc,char *argv[])
{
    int max = 0;
    struct tnode *tree = NULL;
    struct tnode *tr = NULL;

    if (argc > 1) {
	if (!strcmp(argv[1], "-i"))
		freopen(argv[2], "r", stdin);
        else if (!strcmp(argv[1], "-o"))
		freopen(argv[2], "w", stdout);
    }
    if (argc > 3)
	if (!strcmp(argv[3], "-o"))
		freopen(argv[4], "w", stdout);

    tree = dotree();

    maxcount(tree, &max);
    /*dotree2(tree, tr);
    doend(tr);
    treeprint(tr);*/
    int i;
    for(i = max; i > 0; i--) printtree(tree, i);
    deltree(tree);
    return 0;
}