#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<math.h>
#include"first.h"
// inlcude -lm in makefile

//int counter=0;
int memReads = 0;
int memWrites = 0;
int hit = 0;
int miss = 0;
int prefetchReads = 0;
int prefetchMisses = 0;
int prefetchHits = 0;
int prefetchMiss = 0;
void popZero(int, int);


typedef struct cacheBlock{
	int time;
	unsigned long long int address;
	unsigned long long int tag;
	struct cacheBlock* next;
}cacheBlock;

unsigned long long int** table;

int main(int argc, char** argv){
	int sets;
	int cacheSize = atoi(argv[1]);
	char* associativity = argv[2];
	//char* type = argv[3];
	int assoc;
	int blockSize = atoi(argv[4]);
	FILE* fp = fopen(argv[5], "r");
	int type = cacheType(argv[2]);
	printf("cachetype: %d\n", type);
	//printf("0\n");
	if(type>1){
	//	printf("1\n");
		associativity = strchr(associativity,':');
		char tchar = associativity[1];
		associativity = strchr(associativity,tchar);
		assoc = atoi(associativity);
		sets = cacheSize/(blockSize*assoc);
	} else if(type == 0){
		//printf("2\n");
		assoc = cacheSize/blockSize;
		sets = 1;
	}
	else{
		//printf("3\n");
		sets = cacheSize/blockSize;
		assoc = 1;
	}
	//printf("building w assoc: %d\n", assoc);
	buildTable(sets, assoc);
	//printf("simulating\n");
	simulate(fp, type, blockSize, assoc, sets);
	return 0;
}


void simulate(FILE* fp, int type, int blockSize, int assoc, int set){
	unsigned long long int address;
	char action;
	unsigned long long int tag;
	//int tempTag;
	int temp;
	unsigned long long int index;
	int blockOffset = getBlockOffset(blockSize);
	int indexOffset = getIndexOffset(set);
	//printf("4\n");
	while(fscanf(fp, "%*x: %c 0x%llx", &action, &address) > 0){
			//as it reads thru the file it takes the tag and index and stores them
			index = getIndex(blockOffset, indexOffset, address);
			tag = getTag(address, blockOffset, indexOffset);
			//then accesses cache based off of index and search for tagid (so make helper to find tagid)
			temp = matchTag(index, tag, blockOffset+indexOffset);
			int vals = sizeof(table[index][0])/sizeof(unsigned long long int);
			//is miss
			//printf("determining\n");
			if(temp == -1){
				//increment miss counter
				miss++;
				if(action == 'R')
					memReads++;
				if (action == 'W'){
					memWrites++;
					memReads++;
					//memReads++;
				}
				//printf("addin\n");
				if(vals==assoc){
					addNode(index, address);
				}
				else if (vals < assoc)
					addNode(index, address);
				else if (vals == 0){
					addNode(index, address);
				}
			}
			//is a hit
			else{
				hit++;
				if (action == 'W'){
					memWrites++;
				}
			}
	}
	printf("no-prefetch\n");
	printf("Memory reads: %d\n", memReads);
	printf("Memory writes: %d\n", memWrites);
	printf("Cache hits: %d\n", hit);
	printf("Cache misses: %d\n", miss);
}

void buildTable(int sets, int assoc){
	table = (unsigned long long int**) malloc(sets*sizeof(unsigned long long int*));
	int i;
	for(i = 0; i < sets; i++){
		table[i] = (unsigned long long int*) malloc(assoc * sizeof(unsigned long long int));
	}
	//printf("4\n");
}

int matchTag( unsigned long long int bucket, unsigned long long int tag, unsigned long long int offset){
	unsigned long long int* temp = table[bucket];
	int i;
	//printf("size: %lx\n",sizeof(temp)/sizeof(unsigned long long int) );
	for(i = 0; i< sizeof(temp)/sizeof(unsigned long long int); i+=1){
		if(getTag(table[bucket][i], bucket,offset ) == tag){
			printf("return 1\n");
			return 1;
		}
	}
	return -1;
}

int cacheType(char* name){
	int temp;
	char* t = strchr(name,':');
	if(t!=NULL){
		t = strchr(t, t[1]);
		//is of type assoc:#
		temp = atoi(t);
		if(temp%2==0)
			return temp;
		else
			return -1;
	}
	else if (strcmp(name, "assoc")==0){
		//is of type assoc so 1 set
		return 0;
	}
	//is of type direct map
	return 1;
}

unsigned long long int getIndexOffset(int set){
	return log(set)/log(2);
}

unsigned long long int getBlockOffset(int blockSize){
	return log(blockSize)/log(2);
}

unsigned long long int getIndex(unsigned long long int blockOffset, unsigned long long int indexOffset, unsigned long long int address){
        return (address>>blockOffset)%(1<<indexOffset);
}

unsigned long long int getTag(unsigned long long int address, unsigned long long int index, unsigned long long int offset) {
	return address>>(offset+index);
}

void addNode(unsigned long long int index, unsigned long long int address){
	shiftVals(index);
	table[index][0] = address;
}

void shiftVals(unsigned long long int index){
	unsigned long long int* temp = table[index];
	if(sizeof(temp)/sizeof(unsigned long long int) == 1)
		return;
	int i;
	for( i = sizeof(table[index])/sizeof(unsigned long long int)-1; i>-1; i++){
			temp[i+1] = temp[i];
	}
}

void popZero(int assoc, int set){
	int i, j;
	printf("assoc: %d , set: %d \n", assoc, set);
	printf("v: %lx, h: %lx\n",sizeof(table)/sizeof(unsigned long long int*), sizeof(table[0])/sizeof(unsigned long long int) );
	for(i = 0; i < set; i+=1){
		for(j=0; j < assoc; j+=1){
			printf("i: %d, j: %d", i ,j);
			table[i][j] = 0;
		}
	}
}
	/*if(mode == 0){
		(*head) = new;
		return;
	}
	else if(mode == 1){
		while(temp->next != NULL){
			temp = temp->next;
		}
		temp->next = new;
		return;
	}
	//when mode = -1
	if(old == NULL){
		printf("error\n");
		return;
	}
	while(temp->next != old){
		temp = temp->next;
	}
	if(old->next != NULL){
		new -> next = old->next->next;
	}else
		new->next = old;

	temp ->next = new;*/

//define hash table global hit and miss counters and mem reads
//buckets are defined by input so to find # of buckets = C/A*B
//define load var function use fscanf("%llu")
//
