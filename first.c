#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<math.h>
#include"first.h"
// inlcude -lm in makefile

int counter=0;
int memReads = 0;
int memWrites = 0;
int hit = 0;
int miss = 0;
int prefetchReads = 0;
int prefetchMisses = 0;
int prefetchHits = 0;
int prefetchMiss = 0;

struct cacheBlock{
	int time;
	unsigned long long int address;
	unsigned long long int tag;
	struct cacheBlock* next;
}cacheBlock;

struct cacheBlock** table;

int main(int argc, char** argv){
	int sets;
	int cacheSize = atoi(argv[1]);
	char* associativity = argv[2];
	//char* type = argv[3];
	int assoc;
	int blockSize = atoi(argv[4]);
	FILE* fp = fopen(argv[5], "r");
	int type = cacheType(argv[2]);
	//printf("0\n");
	if(type>1){
	//	printf("1\n");
		associativity = strchr(associativity,':');
		char tchar = associativity[1];
		associativity = strchr(associativity,tchar);
		assoc = atoi(associativity);
		sets = blockSize*cacheSize/type;
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
	buildTable(sets, assoc);
	simulate(fp, type, blockSize, assoc, sets);
	return 0;
}


void simulate(FILE* fp, int type, int blockSize, int assoc, int set){
	unsigned long long int address;
	char action;
	unsigned long long int tag;
	//int tempTag;
	struct cacheBlock* temp;
	unsigned long long int index;
	struct cacheBlock* currBucket;
	struct cacheBlock* tempBlock;
	int offset = log2(blockSize);
	//printf("4\n");
	while(fscanf(fp, "%*x: %c 0x%llx", &action, &address) > 0){
			//as it reads thru the file it takes the tag and index and stores them
			index = (address>>offset)%(1<<(int)(log2(set)));
			tag = address>>(offset+index);
			printf(" set: %d offset: %lf index: %llx tag: %llx \n",set, log2(blockSize) ,index, tag);
			//then accesses cache based off of index and search for tagid (so make helper to find tagid)
			temp = matchTag(index, tag);
			currBucket = table[index];
			tempBlock = (struct cacheBlock*)malloc(sizeof(cacheBlock));
			tempBlock->next = NULL;
			tempBlock->time = counter;
			tempBlock->address = address;
			tempBlock->tag = tag;
			//is miss
			if(temp ==NULL){
				//increment miss counter
				miss++;
				if(action == 'R')
					memReads++;
				if (action == 'W'){
					memWrites++;
					//memReads++;
				}
				addNode(&currBucket, NULL, tempBlock, 0);
				counter++;
			}
			//is a hit
			else{
				//increment hit counter
				struct cacheBlock* t = findFirst(currBucket);
				hit++;
				//if(action == 'R'){
					//memReads++;
					//printf("memReads++\n" );
			//	}
				 if (action == 'W'){
					memWrites++;
				//	printf("memReads++\n");
				}
				if(countLL(currBucket) == assoc){
					addNode(&currBucket, t, tempBlock, -1);
					//printf("5\n");
					counter++;
				}
				else{
					addNode(&currBucket, t, tempBlock, 1);
					counter++;
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
	table = (struct cacheBlock**) malloc(sets*sizeof(struct cacheBlock*));
	int i;
	for(i = 0; i < sets; i++){
		table[i] = (struct cacheBlock*) malloc(assoc * sizeof(struct cacheBlock));
	}
	//printf("4\n");
}

struct cacheBlock* matchTag( int bucket, unsigned long long int tag){
	struct cacheBlock* start = table[bucket];
	while(start != NULL){
		if(start->tag == tag)
			return start;
		start = start-> next;
	}
	return NULL;
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
void addNode(struct cacheBlock** head, struct cacheBlock* old, struct cacheBlock* new, int mode){
	struct cacheBlock* temp = (*head);
	if(temp->next == NULL){
		(*head) = new;
		return;
	}
	if(mode == 0){
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
	while(temp->next != old){
		temp = temp->next;
	}
	if(old == NULL){
		printf("bitch wut");
	}
	else if(old->next != NULL){
		new -> next = old->next->next;
	}else
		new->next = old;

	temp ->next = new;
}

struct cacheBlock* findFirst(struct cacheBlock* head){
	if(head->next == NULL)
		return head;
	struct cacheBlock* thead = head;
	int smallest;
	struct cacheBlock* smallestNode;
	while(thead!=NULL){
		//printf("6\n");
		if(thead->time < smallest){
			smallest = thead->time;
			smallestNode = thead;
		}
		thead = thead->next;
	}
	return smallestNode;
}

int countLL(struct cacheBlock* head){
	int count = 0;
	struct cacheBlock* temp = head;
	while(temp != NULL){
		count++;
		temp = temp->next;
	}
	return count;
}
//define hash table global hit and miss counters and mem reads
//buckets are defined by input so to find # of buckets = C/A*B
//define load var function use fscanf("%llu")
//
