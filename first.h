#include<stdio.h>
#include<stdlib.h>
#include<math.h>

void simulate(FILE*, int, int ,int,int);
void buildTable(int, int);
int matchTag(unsigned long long int,unsigned long long int, unsigned long long int);
int cacheType(char*);
void addNode(unsigned long long int,unsigned long long int);
unsigned long long int getIndexOffset(int);
unsigned long long int getBlockOffset(int);
unsigned long long int getIndex(unsigned long long int , unsigned long long int, unsigned long long int);
unsigned long long int getTag(unsigned long long int, unsigned long long int, unsigned long long int);
void shiftVals(unsigned long long int);
