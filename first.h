#include<stdio.h>
#include<stdlib.h>
#include<math.h>

void simulate(FILE*, int, int ,int,int);
void buildTable(int, int);
struct cacheBlock* matchTag(int, unsigned long long int);
int cacheType(char*);
void addNode(struct cacheBlock**, struct cacheBlock*, struct cacheBlock*, int);
struct cacheBlock* findFirst(struct cacheBlock*);
int countLL(struct cacheBlock*);
